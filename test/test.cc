// #include "bencode.h"
#include <string>
#include <sstream>
#include <iostream>
#include "torrent.h"
#include <fstream>

int main()
{
    // BDecoder decoder("debian.torrent");
    // decoder.Decode();
    // auto value = decoder.Cast<BDecoder::DictValue>();
    // for (auto &&i : value)
    // {
    //     std::cout<<i.first<<std::endl;
    // }
    // auto info = decoder.Cast<BDecoder::DictValue>(value["info"]);
    // for (auto &&i : info)
    // {
    //     std::cout<<i.first<<std::endl;
    // }
    Torrent t("debian.torrent");
    std::cout << t.ToString() << std::endl;
    t.FindPeers();
}