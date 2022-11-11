# INSTALL PANDOC RELATIVE


1. installing pandoc

download deb from [pandoc release](https://github.com/jgm/pandoc/releases/tag/2.19.2)

```
wget https://github.com/jgm/pandoc/releases/download/2.19.2/pandoc-2.19.2-1-amd64.deb

sudo dpkg -i pandoc-2.19.2-1-amd64.deb

pandoc --version # check pandoc version 

```

2. install tex relative

```
apt install texlive texlive-latex-extra texlive-latex-recommended texlive-xetex texlive-lang-chinese
```

3. install chinese fonts

```
apt install fontconfig && fc-list :lang=zh
```

since the size of font file is kind of huge，didn't find a good way to restore it

you can download it from network or 

copy it in your windows system, default location is C:\Windows/Fonts

after you put font files in a directory successfully

exec `mkfontscale` in fonts directory to generate font index

you can you `fc-list :lang=zh` to check the font installed in your system






