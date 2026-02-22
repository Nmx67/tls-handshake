#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include "./tls_connection.hpp"
#include "win32_util.h"


/*
TLS handshake algorithm

cipher TLS_RSA_WITH_AES_128_CBC_SHA on TLSv1.2 (RFC5246), which entails:
   - The RSA key exchanging strategy
   - The AES-128 algorithm for encrypting application data, with added security using the CBC method
   - SHA1 hashing for generating application data checksums



P.S.

Faiblesses cryptographiques TLS 1.2 et des vulnérabilités ou attaques qui leur sont associées :
    Transport de clé RSA : ne fournit pas de confidentialité persistante
    Chiffrement en mode CBC : attaques BEAST et Lucky 13
    Chiffre de flux RC4 : non sécurisé pour une utilisation dans HTTPS
    Groupes Diffie-Hellman arbitraires : CVE-2016-0701
    Chiffrement export : attaques FREAK et LogJam
*/


/*
This example should give the same result as :
curl https://api.binance.com/api/v3/time --connect-to api.binance.com:443:143.204.193.208:443
or
curl https://api.binance.com/api/v3/time --connect-to api.binance.com:443:143.204.193.208:443 --http1.1 --tls-max 1.2
*/



/*
TLS handshake process can be divided into two stages:
 1- Hello stage: The client receives the server’s public certificate for 
    secure handshaking (Certificates packet).
 2- Key exchange stage: The client sends data for generating the AES key, 
    using the server’s public certificate (RSA) to encrypt it and protect it from prying eyes (Client Key Exchange packet).


*/

int main(int argc, char** argv) {


#ifdef _WIN32
   if (!win_sock_start()) {
      return EXIT_FAILURE;
   }
#endif

    std::string host = "143.204.193.208";
    std::optional<std::string> hostname = "api.binance.com";
    int port = 443;

    auto ctx = TLSContext();
    if (hostname.has_value()) {
        ctx.set_hostname(*hostname);
    }
    auto con = TLSConnection(&ctx, host, port);
    con.connect();
    std::cout << "connected" << std::endl;

    std::string req = "GET /api/v3/time HTTP/1.1\r\nHost: api.binance.com\r\nAccept: */*\r\n\r\n";
    con.send(req);
    auto resp = con.recv()[0];
    std::cout << resp << std::endl;

#ifdef _WIN32
    if (!win_sock_stop()) {
       return EXIT_FAILURE;
    }
#endif
    return EXIT_SUCCESS;

}