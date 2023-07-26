# C++ BT下载客户端
## 项目简介
使用 C++ 开发的简单多线程并发 BT下载客户端，网络部分使用 [boost](https://www.boost.org/) asio完成 TCP 和 UDP 连接和 beast 以支持 http 和 https 请求。
## 项目功能
|功能|已完成|待完成|
| --- | --- | --- |
|🧲 torrent 单文件解析|✅||
|💻 连接 tracker 服务器 |✅||
|🔍 tracker返回值（peer 信息）解析 |✅||
|🤝 peer TCP 握手|✅||
|🌩️ 文件下载||✅|
|📄 uTP 协议支持||✅|
|🌐 DHT 协议支持||✅|
## 第三方库
- [boost](https://www.boost.org/)
- [Crypto++](https://www.cryptopp.com/)
## 参考
- [「Go手写BT下载器」](https://www.bilibili.com/video/BV13P4y1378F)以及 [go-torrent](https://github.com/archeryue/go-torrent)
- [The BitTorrent Protocol Specification](https://www.bittorrent.org/beps/bep_0003.html)
- [uTorrent transport protocol](https://www.bittorrent.org/beps/bep_0029.html)
- [DHT Protocol](https://www.bittorrent.org/beps/bep_0005.html)

