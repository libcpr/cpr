#!/bin/sh

# Generate a CA with a self-signed root certificate that then signs the server certificate
# Based on the OpenSSL Cookbook by Ivan Ristic:
# https://www.feistyduck.com/library/openssl-cookbook/online/
#
# Especially, see chapter 1.5. Creating a private Certification Authority:
# https://www.feistyduck.com/library/openssl-cookbook/online/openssl-command-line/private-ca.html

export KEY_PATH=keys
export CRT_PATH=certificates
export CA_PATH=ca

# Create environment:
mkdir -p $KEY_PATH $CRT_PATH $CA_PATH/db $CA_PATH/private $CA_PATH/certificates
touch $CA_PATH/db/index
openssl rand -hex 16  > $CA_PATH/db/serial

# Generate all keys
openssl genpkey -algorithm ed25519 -out $KEY_PATH/root-ca.key
openssl genpkey -algorithm ed25519 -out $KEY_PATH/server.key
openssl genpkey -algorithm ed25519 -out $KEY_PATH/client.key



# Generate a Certificate Signing Request for the Root CA based on a config file
openssl req -new \
    -config root-ca.cnf -out root-ca.csr \
    -key $KEY_PATH/root-ca.key

# Self-sign the root certificate
openssl ca -batch \
    -selfsign -config root-ca.cnf \
    -extensions ca_ext \
    -in root-ca.csr -out $CRT_PATH/root-ca.crt


# Create a Certificate Signing request for the server certificate
openssl req -new \
    -config server.cnf -out server.csr \
    -key $KEY_PATH/server.key
openssl req -text -in server.csr -noout

# Issue the server certificate
openssl ca -batch \
    -config root-ca.cnf \
    -extensions server_ext \
    -extfile server.cnf -extensions ext \
    -in server.csr -out $CRT_PATH/server.crt \
    -days 1825


# Create a Certificate Signing request for the client certificate
openssl req -new \
    -config client.cnf -out client.csr \
    -key $KEY_PATH/client.key

# Issue the client certificate
openssl ca -batch \
    -config root-ca.cnf \
    -extensions client_ext \
    -in client.csr -out $CRT_PATH/client.crt \
    -days 1825



# Clean up
rm -rf *.csr $CA_PATH
