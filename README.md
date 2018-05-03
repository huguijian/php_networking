PHP TCP/UDP 网络通信扩展
简单的实现php服务端tcp/udp通信扩展。
**example**
$server = new Network('127.0.0.1',9000)  

$server->on('onConnect',function($server, $fd){
  echo "connection open: {$fd}\n";
	
});  

$server->on('onMessage',function($server,$fd){
	$server->send($fd,'hello world!');
	
})
$server->run();  

