#!/bin/bash

mkdir -p test/data
cd test/data

openssl genrsa -out ca.key 2048
openssl req -new -x509 -key ca.key -out ca.crt

openssl genrsa -out key.pem 2048
openssl req -new -key key.pem -out cert.csr
openssl x509 -req -in cert.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out cert.pem

c_rehash .