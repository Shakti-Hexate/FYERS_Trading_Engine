This is a trading engine which uses FYERS cURL apis implemented in C++

Additional Dependencies required:
openssl = used to create SHA256 hash for token generation
curl = used to make Https requests
simdjson = to parse json objects

NOTE:
paste your api and login credentials in the credentials.h file.

refresh token is stored in the refresh_token.txt file which is used to generate the access token , this refresh token expires in 15 days and when that happens delete the refresh_token.txt file to generate a fresh refresh token and create a new file. remeber to generate a fresh authcode manually and pasting it in the credentials file before that.
