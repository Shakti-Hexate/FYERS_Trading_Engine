# Trading Engine using FYERS cURL APIs in C++

## Overview
This is a C++ trading engine that uses FYERS cURL APIs to automate trading. The engine is designed for low-latency operations and high-frequency trading scenarios. It leverages the following external libraries to enhance functionality:

### Dependencies
1. **OpenSSL**: Used to create SHA256 hashes for token generation.
2. **cURL**: Used to make HTTPS requests to FYERS APIs.
3. **simdjson**: Used to parse JSON objects efficiently.

---

## Features
- **Access Token Management**: Automatically handles access token generation using the refresh token.
- **High-Frequency Data Handling**: Designed for high-speed processing of trading data.
- **JSON Parsing**: Efficiently parses API responses using `simdjson`.

---

## Setup

### Steps to Configure

1. **Install Dependencies**:
   - Install OpenSSL, cURL, and simdjson on your system.
     ```bash
     sudo apt install libssl-dev libcurl4-openssl-dev
     git clone https://github.com/simdjson/simdjson.git
     ```

2. **Add API and Login Credentials**:
   - Paste your API key, secret key, and other login credentials in `credentials.h`.

3. **Refresh Token Management**:
   - The `refresh_token.txt` file stores the refresh token, which is used to generate the access token.
   - The refresh token expires every 15 days. When it expires, follow these steps:
     - Delete the `refresh_token.txt` file.
     - Generate a fresh auth code manually from the FYERS portal.
     - Paste the new auth code into `credentials.h`.

4. **Build the Project**:
   - Use CMake to build the project.
     ```bash
     mkdir build && cd build
     cmake ..
     make
     ```

5. **Run the Trading Engine**:
   ```bash
   ./trading_engine
   ```

---

## Notes
- **Refresh Token Expiry**:
  - The refresh token is automatically used to generate an access token. However, when the refresh token expires (every 15 days), you need to manually obtain a new auth code and update the credentials file.

- **Performance**:
  - This project is optimized for low-latency trading environments.
  - Ensure your system has sufficient resources to handle high-frequency data.

---

## Contributing
Feel free to submit issues or pull requests to improve the project. Suggestions for optimization and new features are welcome!

---


