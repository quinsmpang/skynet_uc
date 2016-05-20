local skynet = require "skynet"
--[[
	采用下述命令生成公私钥对
	
	openssl genrsa -out rsa_private_key.pem 1024
	
	openssl rsa -in rsa_private_key.pem -pubout -out rsa_public_key.pem


]]

	local privpem = [[-----BEGIN RSA PRIVATE KEY-----
MIICXAIBAAKBgQDHorPVB1Nx2BXPFi5Uc2gmXzmC4ptMUvAZbz7Eg5ZV4OkvHH2g
rJVZJU2paDRtGHBrvAJl2YnQaagaYVISi7L0ZB4sTKxbRQd/fr/1kGPL/rgrB1Jk
E5pcXFwn+oH511n+XpRXDmaZ2epYnAzKLD//CD/iwK/GCypCiZo/gACHhQIDAQAB
AoGAB86qH8OolMQxnias6yqUQSPqON8p2aNItoXuO3s17ufb1rbThuwY7yKRVfW4
TyfshZoVsev/AsKPC3EQ557FAc4DHWRxhbpuLb8cH8XFD4YJJLKNQrPgwQ45+kwL
bNzzrRn3J7xPy0ONEIbSUuyFkHo/uEmsl3jQbC/+OYFRBOECQQD3nf/YZKRsrhkq
54OhkIbHPW705Hyi/DXYq5pGcVGIaCtOvNpD8wVn9GudsUHB9qj3kVJX31HGu7kF
8kWIyhCtAkEAzmTdfa16Uh2m7EegY5roN1AcfU7qOQmIAIo+/k5LYStaCh9u3EcT
D/FSySKGbtUSkBzhky07F/6RdqFVmqs1OQJAHnL8ciS2H3MtPxxnEGgQWtHQIicy
uGwhRvqbJwxqRUbiBHWutWpeANGKpkKU3oAxePgS0yFwEeslrniDWnXWpQJAGN3e
Mb4s9qfL7LvUfyuyvrdQN2jCn4yebjI51LnqKxLmDkIrQS7vBQS2TLV0LPZO/j38
j5brf8CUFg7qGXB3QQJBAOHehkRX99S5TxAnRZ5VhBg+ADopPCZEmsHLU5fasQb0
JeaevH601cbgVhK9nYyGvG4hauAhShY9iLy66rLg52s=
-----END RSA PRIVATE KEY-----]]

	local pubpem = [[-----BEGIN PUBLIC KEY-----
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDHorPVB1Nx2BXPFi5Uc2gmXzmC
4ptMUvAZbz7Eg5ZV4OkvHH2grJVZJU2paDRtGHBrvAJl2YnQaagaYVISi7L0ZB4s
TKxbRQd/fr/1kGPL/rgrB1JkE5pcXFwn+oH511n+XpRXDmaZ2epYnAzKLD//CD/i
wK/GCypCiZo/gACHhQIDAQAB
-----END PUBLIC KEY-----]]


skynet.start(function()
	print("test start ------------>")

	local sec = require('sec')

--[[

-- base64 test

	local src = '123456'
	local dst = sec.base64_encode(src)
	print(dst)

	local dsrc = sec.base64_decode(dst)
	print(dsrc)

]]

--[[

--rsa crypto test

	local src = '123456'
	local typ = 2
	local bs = sec.rsa_public_encrypt(src, pubpem, typ)
	local dst = sec.base64_encode(bs)
	print("Encrypt with public key:", dst)

	local dbs = sec.base64_decode(dst)
	local dsrc = sec.rsa_private_decrypt(dbs, privpem)
	print("Decrypt with private key:", dsrc)
]]



--[[

--rsa sign test

	local src = '123456'

	local bs = sec.rsa_private_sign(src, privpem)
	local sign = sec.base64_encode(bs)
	print(sign)

	local dbs = sec.base64_decode(sign)

	local typ = 2
	local ok = sec.rsa_public_verify(src, dbs, pubpem, typ)

	print("Verify result: ", ok)

]]


-- hmac test
	local src, key = '123456', '112233'
	local dst = sec.hmac_sha1_encode(src, key)
	print(dst)
	print("Verify result:", '06285a0e4a99a56f7f9d1e239acad4de7c79ebe9' == dst)

	print("test end <------------")

end)
