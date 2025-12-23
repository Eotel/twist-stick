# Twist Stick

ESP32ベースのIMU回転トラッカー。M5Stackデバイス（M5StickC, M5StickC Plus, M5Stack Core2）で動作し、Mahony AHRSフィルタを使用して3軸の累積回転を追跡、OSCでデータを送信します。

## Features

- 6DoF IMU（加速度計 + ジャイロスコープ）によるリアルタイム姿勢推定
- Mahony AHRSフィルタによるクォータニオン出力（100Hz）
- 3軸の累積回転追跡（角度、回転数、累積ラジアン）
- OSCプロトコルでのデータ送信
- Webベースの設定UI（APモード）
- ジャイロオフセットキャリブレーション（NVS保存）
- ディスプレイの自動スリープ

## Requirements

- PlatformIO
- M5StickC / M5StickC Plus / M5Stack Core2

## Quick Start

```bash
# ビルド（デフォルト: m5stick-c-plus）
pio run

# ファームウェアをアップロード
pio run -t upload

# シリアルモニタ
pio device monitor
```

### デバイス別ビルド

```bash
pio run -e m5stick-c-plus   # M5StickC Plus
pio run -e m5stick-c        # M5StickC
pio run -e m5stack-core2    # M5Stack Core2
```

## Usage

### ボタン操作

| ボタン | 操作 | 機能 |
|--------|------|------|
| A | 短押し | 回転リセット |
| B | 短押し | ジャイロキャリブレーション |
| B | 長押し (2秒) | 設定モード開始 |

### 設定モード

1. ボタンBを2秒間長押しで設定モードに入る
2. WiFi AP "TwistStick-Config" に接続
3. ブラウザで `192.168.4.1` にアクセス
4. WiFi接続先とOSCターゲットを設定

## OSC Protocol

### 送信（デフォルトポート: 9000）

| Address | Arguments | Description |
|---------|-----------|-------------|
| `/twist-stick/quaternion` | 4 floats `[w, x, y, z]` | 姿勢クォータニオン |
| `/twist-stick` | 9 floats `[angle[3], turns[3], theta[3]]` | 回転データ |

### 受信（デフォルトポート: 9001）

| Address | Arguments | Description |
|---------|-----------|-------------|
| `/twist-stick/reset` | なし | 回転リセット |
| `/twist-stick/calibrate` | なし | ジャイロキャリブレーション開始 |

## Project Structure

```
twist-stick/
├── src/
│   ├── main.cpp          # メインループ
│   ├── config.h          # 定数設定
│   ├── imu/              # IMU読み取り、Mahony AHRS
│   ├── twist/            # 回転計算
│   ├── osc/              # OSC送受信
│   ├── network/          # WiFi管理、設定Webサーバー
│   ├── storage/          # NVS設定保存
│   └── display/          # 画面表示管理
├── data/                 # SPIFFSファイル
└── platformio.ini        # PlatformIO設定
```

## Related Projects

- [twist-stick-viewer](https://github.com/Eotel/twist-stick-viewer) - Godot 4製のOSCビューワーアプリ

## License

MIT
