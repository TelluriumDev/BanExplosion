# BanExplosion

一个可以制止爆炸，或是让爆炸变得无害的插件。

## 功能

- 可分维度，分生物/实体配置爆炸选项
- 可控制的参数丰富，参考[配置](#配置)

## 安装

在Releases或者Action中下载最新版本的压缩包，并把里面的内容解压到`plugins`文件夹中。

## 配置

在`./plugins/BanExplosion/config/config.json`中，你可以设置拦截的爆炸以及爆炸参数。

```jsonc
{
    "version": 2, // 配置文件版本
    "GlobalSetting": { // 全局设置，如果并未在下方的代码找到对应配置，则使用此配置
        "AllowExplosion": false, // 是否允许爆炸
        "AllowDestroy": false,   // 是否允许破坏方块
        "AllowFire": false,      // 是否允许生成火
        "MaxRadius": 3.4028234663852886e+38 // 爆炸最大半径
    },
    "ExplosionSetting": { // 自定义爆炸配置
        "Overworld": { // 维度，你可以根据维度名来配置，原版的维度名有：Overworld, Nether, TheEnd
            "DefaultSetting": { // 每个维度的默认配置
                "AllowExplosion": true,
                "AllowDestroy": false,
                "AllowFire": false,
                "MaxRadius": 3.4028234663852886e+38
            },
            "CustomSettings": { // 维度内的部分特殊设置，这些配置会覆盖所有默认配置
                "minecraft:tnt": {
                    "AllowExplosion": true,
                    "AllowDestroy": true,
                    "AllowFire": false,
                    "MaxRadius": 3.4028234663852886e+38
                }
            }
        }
    }
}
```
配置顺序为：
1. 插件首先会寻找配置文件内是否配置了维度，如果没有则使用`GlobalSetting`。
2. 如果找到了维度，则优先在`CustomSettings`内寻找匹配的配置项并使用，如没有则使用维度的默认配置。

维度名字并非写死，如果有多维度需求仅添加维度名即可。