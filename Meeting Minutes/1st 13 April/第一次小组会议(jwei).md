# 第一次小组会议

## 会议概况

- 主讲人：胡文杰
- 记录：魏嘉琪
- 参会人员：陈文懿、王思博、曾庆哲、王雪峰
- 会议时间：2024/4/13 星期六 下午 2：00——4：00

## 主讲人分享
游戏：[**异常**](https://www.taptap.cn/app/59622)

游戏：[**Human Resource Machine**](https://store.steampowered.com/app/375820/Human_Resource_Machine/#app_reviews_hash)

生产力工具：**双拼**

- 关键词：

  效率

  小众
- 简介：
  双拼是汉语拼音输入法的一种编码方案。

  相对于全拼而言，使用双拼输入汉字时只需输入一个代表声母的字母，一个代表韵母的字母，就可以打出任意一个中文文字。


![](https://files.mdnice.com/user/63102/9f65264c-befe-48a1-a715-fcfe11a718e2.png)

![](https://files.mdnice.com/user/63102/a9faa804-5a6e-4400-89f4-611da16aeb2d.png)

- 优点：
  比全拼快，比五笔简单

- 缺点：需要记忆键位，有学习成本

- 方案：

自然码

![](https://files.mdnice.com/user/63102/f547b04a-b316-48e5-bc9f-e5a4488d343a.png)

   微软双拼

![](https://files.mdnice.com/user/63102/ed945ad6-6783-4688-8468-957c8c5ace7e.png)

小鹤双拼


![](https://files.mdnice.com/user/63102/6098e537-3edd-47d8-b711-cc2ff337811b.png)

- 学习建议：

各主流输入法在设置里都可以切换双拼模式

先在手机端熟悉键位，再转移到PC端建立肌肉记忆

手机可使用讯飞输入法，皮肤库有双拼助记皮肤

PC端微软拼音输入法内置的双拼方案比较少，导入小鹤双拼可按

  - 以下方法：

新建文本文档

写入以下文本


```markdown
Windows Registry Editor Version 5.00
[HKEY_CURRENT_USER\Software\Microsoft\InputMethod\Settings\CHS]
"LangBar Force On"=dword:00000000
"Enable Double Pinyin"=dword:00000001
"EmoticonTipTriggerCount"=dword:00000001
"HapLastDownloadTime"=hex(b):eb,69,29,59,00,00,00,00
"UserDefinedDoublePinyinScheme0"="FlyPY*2*^*iuvdjhcwfg xmlnpbksqszxkrltvyovt"
"DoublePinyinScheme"=dword:0000000a
"UDLLastUpdatedTime"="2017-05-27 22:01:40"
"UDLCount"=dword:0000018b
"UDLVisibleCount"=dword:0000018b

```

修改文件拓展名为.reg

双击运行

- 关联阅读：

[做少数派中的少数派：双拼输入快速入门 - 少数派 (sspai.com)](https://sspai.com/post/32809)

[让双拼不再是只属于少数人的输入方式 - 少数派 (sspai.com)](https://sspai.com/post/42667)

公众号：**正面连接**

- 关键词：

面对复杂

- 简介：

正面连接是专注于非虚构和特稿的新媒体品牌，旨在呈现现实中易被忽略的重要话题，在人与故事的切面后展现当代中国的时代脉络，主要受众为新中产、知识分子、青年学生等关注世界变化的人群。

- 经典文章：

[和衡水中学在一起的2557天 (qq.com)](https://mp.weixin.qq.com/s/xJw7gmhw9PjUA7xbVoUTQA)

[两个世界：父亲与自杀的少年 (qq.com)](https://mp.weixin.qq.com/s/z_2WMTbUq-98gF3pbjmNNw)

[我的朋友庞麦郎 (qq.com)](https://mp.weixin.qq.com/s/nFAhtLprPfJNSsglLEAGrA)

[40%：“毫不重要”的中职世界 (qq.com)](https://mp.weixin.qq.com/s/KyBLn5XX0-8iM8hRHZKmGg)

[最好的中学，最后的改革 (qq.com)](https://mp.weixin.qq.com/s/X_UwmbvsHe7oKd7P8qZHTw)

[她在MU5735上，她也是我的姐妹 (qq.com)](https://mp.weixin.qq.com/s/ntyQSlE1RqU3QAOmU9Z8ww)

[在上海方舱的19个日夜 (qq.com)](https://mp.weixin.qq.com/s/_8EeMYE1eZZKO-qcl97YFg)

[中国式摇滚席卷石家庄 (qq.com)](https://mp.weixin.qq.com/s/zIAL__p9FM3qyPw0tuTsxg)

以及一些被404的更经典文章...

- 关联阅读：

[何以连接 | 长报道 (qq.com)](https://mp.weixin.qq.com/s/xV2UxhD5mlELI63t_UgiTA?poc_token=HEx6F2ajsQ92uo1-KEI6XIM0MlmfNavquP1TJkRT)

漫画：**チ。-地球の運動について-**

- 关键词：

知性与暴力

真理与 ⌈?⌋

- 关联阅读：

[几年来最让我感动的漫画神作，中文版终于出全了 (qq.com)](https://mp.weixin.qq.com/s/lzobS7u4Iev2bCWahvtFXg)

[《チ。―关于地球的运动―》：何谓伟大？ - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/473795903)

[越 过 星 空 |《关于地球的运动》漫评 (qq.com)](https://mp.weixin.qq.com/s/M1mg3eApUXNddQ1l2mWc2A)


## 组员分享


复盘了第一个C项目词法分析器，交流了代码逻辑和实现过程。

有如下一些优化：

- 字符串判断

```C
static Token string(void) {
	while (!isAtEnd() && peek() != '"') {
		if (peek() == '\n') {
			scanner.current = scanner.start + 1;
			return errorToken("Without string after '\"'.");
		}
		advance();
	}
	if (isAtEnd()) {
		scanner.current = scanner.start + 1;
		return errorToken("Without string after '\"'.");
	}
	advance();
	return makeToken(TOKEN_STRING);
}
```

- 实现三步运算符判断

```
case '<':
		if (match('=')) return makeToken(TOKEN_LESS_EQUAL);
		else if (match('<')) {
			if (match('=')) return makeToken(TOKEN_LESS_LESS_EQUAL);
			else return makeToken(TOKEN_LESS_LESS);
		}
		else return makeToken(TOKEN_LESS);
	case '>':
		if (match('=')) return makeToken(TOKEN_GREATER_EQUAL);
		else if (match('>')) {
			if (match('=')) return makeToken(TOKEN_GREAER_GREATER_EQUAL);
			else return makeToken(TOKEN_GREAER_GREATER);
		}
		else return makeToken(TOKEN_GREATER);
```

- 跳过空白字符

```
static void skipWhitespace() {
    for (;;) {
        if (peek() == ' ' || peek() == '\n' || peek() == '\r' || peek() == '\t' || peek() == '/' && peekNext() == '/') {
            if (peek() == '\n' || peek() == '/' && peekNext() == '/') scanner.line++;
            scanner.current++;
        }
        else
            break;
    }
}
```

## 圆桌时刻

讨论了学习目标和个人发展目标等。


## 小结

开得很好，下次还来。φ(゜▽゜*)♪
