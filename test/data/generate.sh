#---------------------
# Source:
# https://www.makethenmakeinstall.com/2014/05/ssl-client-authentication-step-by-step/
#---------------------

# Generate a certificate authority (CA) cert
openssl req -newkey rsa:4096 -keyform PEM -keyout ca.key -x509 -days 3650 -outform PEM -out ca.cer

# Generate a server private key
openssl genrsa -out server.key 4096

# Use the server private key to generate a certificate generation request
openssl req -new -key server.key -out server.req -sha256

# Use the certificate generation request and the CA cert to generate the server cert
openssl x509 -req -in server.req -CA ca.cer -CAkey ca.key -set_serial 100 -extensions server -days 1460 -outform PEM -out server.cer -sha256

# Clean up – now that the cert has been created, we no longer need the request
rm server.req

# Generate a private key for the SSL client
openssl genrsa -out client.key 4096

# Use the client’s private key to generate a cert request
openssl req -new -key client.key -out client.req

# Issue the client certificate using the cert request and the CA cert/key
openssl x509 -req -in client.req -CA ca.cer -CAkey ca.key -set_serial 101 -extensions client -days 365 -outform PEM -out client.cer

# Clean up
rm client.req
