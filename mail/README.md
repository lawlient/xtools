# 邮件发送脚本


## 依赖

- sshpass
- gpg
- sendEmail

gpg通常ubuntu系统默认安装支持，通过如下命令安装sshpass和sendEmail工具。
```bash
sudo apt install sshpass sendemail
```

## 创建邮件地址配置

`jarvis -c`通过vim编辑配置文件。

- SENDER： 发件箱地址
- STMP_ADDR: 邮件服务器，默认使用smtp.163.com，自行修改
- RECEIVER: 收件箱地址

## 创建发件箱密码加密文件


通过gpg实现密码明文加密。可自行尝试其他加密工具。

### 生成gpg秘钥

`gpg --gen-key`

1. 默认选择RAS加密算法
2. 秘钥长度，过期时间可以使用默认值
3. 输入用户名和邮件地址，作为uid
4. 输入口令(2次)

可以通过命令`gpg --list-key`查看已生成的秘钥信息。

### 生成密码密文文件

`jarvis -g`

输入密码后生成加密文件。

## 使用

1. 直接发送文本(前提是字符串非文件)

`jarvis -m "hello world"`

2. 指定邮件文件并发送(若文件不存在会当做情况1处理）

`jarvis -m mailfile`

3. 通过管道重定向标准输入进行发送

`echo <email body> | jarvis`

3. 通过终端标准输入键入并发送

`jarvis`

编辑完成后`CTRL + d`结束编辑并发送。



## 其他

gpg-agent可以缓存口令时长，若出现每次需要输入口令，可以修改agent配置。

1. 编辑配置文件`~/.gnupg/gpg-agent.conf`

修改如下配置：

```
default-cache-ttl 604800
max-cache-ttl 604800
```

2. 重新载入agent

`gpgconf --reload gpg-agent`

或重启agent

`gpgconf kill gpg-agent && gpgconf --launch gpg-agent`


----------

参考[官方文档](https://www.gnupg.org/documentation/manuals/gnupg/Agent-Options.html)
