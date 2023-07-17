// #include "bencode.h"
#include <string>
#include <sstream>
#include <iostream>
#include "torrent.h"

int main()
{
    // BDecoder decoder("test.torrent");
    // decoder.Decode();
    Torrent t("ubuntu.torrent");
    std::cout << t.ToString() << std::endl;
}