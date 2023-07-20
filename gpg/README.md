# gpg加解密


## 背景

开发过程中常常遇到需要加解密的场景，如对密码明文做加密存储。

## 原理

基于gpg的加解密，封装一个bash脚本。


## 依赖


### gpg秘钥生成


```bash
gpg --full-generate-key
```

通过命令查看秘钥生成情况

```bash
gpg --list-keys
```

## 安装

```bash
sudo make install
```

## 使用


### 加密

```bash
secret -e 
```

然后终端输入希望加密的文本，默认当前路径下生成文件enc_file。


### 解密

```bash
secret -d enc_file
```

注意enc_file为待解密文件，且是通过`secret -e`进行加密的，需要替换为目标文件。


