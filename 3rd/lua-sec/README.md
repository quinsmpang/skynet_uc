# skynet Openssl binding

Inspired by project: https://github.com/mashijie/lua-codec

modified from "lua-md5" package origin in 3rd/lua-md5


采用下述命令生成公私钥对
	
	openssl genrsa -out rsa_private_key.pem 1024
	
	openssl rsa -in rsa_private_key.pem -pubout -out rsa_public_key.pem


参见测试lua文件：test_sec.lua


./skynet example/config

test_sec



20160520


