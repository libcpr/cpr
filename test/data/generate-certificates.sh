#!/bin/sh

# Generate a CA with a self-signed root certificate that then signs the server certificate
# Based on the OpenSSL Cookbook by Ivan Ristic:
# https://www.feistyduck.com/library/openssl-cookbook/online/
#
# Especially, see chapter 1.5. Creating a private Certification Authority:
# https://www.feistyduck.com/library/openssl-cookbook/online/openssl-command-line/private-ca.html

# Create environment:
mkdir db private certs
touch db/index
openssl rand -hex 16  > db/serial

# Generate a Certificate Signing Request for the Root CA based on a config file
openssl req -new -config root-ca.cnf -out root-ca.csr -keyout root-ca.key -passout pass:

# Self-Sign the root certificate
openssl ca -batch -selfsign -config root-ca.cnf -in root-ca.csr -out root-ca.crt -extensions ca_ext -passin pass:


# Create a Certificate Signing request for a server certificate
openssl req -new -config server.cnf -out server.csr -keyout server.key -passout pass:
# Issue the server certificate
openssl ca -batch -days 1825 -config root-ca.cnf -in server.csr -out server.crt -extensions server_ext -passin pass:

# Create a Certificate Signing request for a client certificate
openssl req -new -config client.cnf -out client.csr -keyout client.key -passout pass:
# Issue the client certificate
openssl ca -batch -days 1825 -config root-ca.cnf -in client.csr -out client.crt -extensions client_ext -passin pass:


# Clean up
rm -rf db certs *.csr private

