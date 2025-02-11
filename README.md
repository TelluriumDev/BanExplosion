# BanExplosion

一个可以制止爆炸，或是让爆炸变得无害的插件。

## 功能

- 可自由设置实体
- 可控制的爆炸参数
- 可自定义拦截爆炸

## 安装

在安装之前，确保你安装了`ilistenattentively`插件。

之后，在Releases或者Action中下载最新版本的压缩包，并把里面的内容解压到`plugins`文件夹中。

## 配置

在`plugins/BanExplosion/config/config.json`中，你可以设置拦截爆炸的实体，以及爆炸参数。

```json
{
    "version": 1, // 版本
    "PluginEnabled": true, // 插件总开关
    "defaultSetting": { // 默认设置
        "allowExplosion": true, // 是否允许爆炸
        "allowDestroy": true, // 是否允许破坏
        "allowFire": true // 是否产生火焰  
    },
    "explosionSetting": {
        "minecraft:tnt": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        // 需要注意的是，床和重生锚只有第一个开关有作用
        // 受底层限制，我们做不了太多。
        "minecraft:bed": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:respawn_anchor": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:tnt_minecart": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:creeper": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:ender_dragon": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:end_crystal": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:wither_skull": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:wither_skull_dangerous": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:fireball": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:dragon_fireball": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        },
        "minecraft:wither": {
            "allowExplosion": true,
            "allowDestroy": false,
            "allowFire": false
        }
    }
}
```