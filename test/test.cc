// #include "bencode.h"
#include <string>
#include <sstream>
#include <iostream>
#include "torrent.h"
#include <fstream>
#include "http.h"
#include "ServicePool.h"

int main()
{
    // io_context ioc;
    auto &spool = ServicePool::GetInstace();
    // std::shared_ptr<Torrent> t(new Torrent("debian.torrent", 1234, spool.GetService()));
    Torrent t("ubuntu.torrent", 1234);
    t.Parse();
    std::cout << t.ToString() << std::endl;

    t.FindPeers();
    // Http::GetInstance().Handle(t->GetQuery(), t);
    spool.Stop();
}