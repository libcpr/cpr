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


# Generate a Certificate Signing Request for the Root CA based on a config file
openssl req -new -config root-ca.cnf -out root-ca.csr -keyout $KEY_PATH/root-ca.key -passout pass:

# Self-Sign the root certificate
openssl ca -batch -selfsign -config root-ca.cnf -in root-ca.csr -out $CRT_PATH/root-ca.crt -extensions ca_ext -passin pass:


# Create a Certificate Signing request for a server certificate
openssl req -new -nodes -config server.cnf -out server.csr -keyout $KEY_PATH/server.key -passout pass:
# Issue the server certificate
openssl ca -batch -config root-ca.cnf -in server.csr -out $CRT_PATH/server.crt -extensions server_ext -passin pass:

# Create a Certificate Signing request for a client certificate
openssl req -new -config client.cnf -out client.csr -keyout $KEY_PATH/client.key -passout pass:
# Issue the client certificate
openssl ca -batch -config root-ca.cnf -in client.csr -out $CRT_PATH/client.crt -extensions client_ext -passin pass:


# Clean up
rm -rf *.csr $CA_PATH

