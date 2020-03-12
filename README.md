# SoftUART

PCIntVect による SoftwareSerial の別実装

このモジュールは、
[SoftwareSerial](https://www.arduino.cc/en/Reference/SoftwareSerial) から fork して、
[PCIntVect](https://github.com/askn37/PCIntVect/) を使うようにしたものである。
使用方法はオリジナルと代わりはない。
しかし、オリジナルではヘッダファイルをインクルードした時点で複数ある
PCINT 割り込みがすべて専有され、他の用途で自由に再定義することが出来なくなってしまうが
本モジュールでは RX受信ピンに割り当てられた PCINT 割り込み（のポートグループ）以外は、
attachPCInterrupt() 関数で任意に割り当て、利用することが可能になっている。
さらに可能であれば、attachInterrupt() を使用して PCINT を空けておくこともできる。

- megaAVR-0系 tinyAVR-0/1/2系 では使用できない。

## Arduino IDE への導入

1. .ZIPアーカイブをダウンロードする。[Click here](https://github.com/askn37/SoftUART/archive/master.zip)

2. ライブラリマネージャで読み込む

    スケッチ -> ライブラリをインクルード -> .ZIP形式のライブラリをインストール...

## 使い方

使用方法は SoftwareSerial と変わりはない。

```c
#include <SoftUART.h>

// RX、TXピンを指定して SoftwareUart の利用を宣言する
SoftwareSerial mySerial(11, 12);

void setup() {
  // PC との シリアル通信を開く
  Serial.begin(9600);
  while (!Serial) {
    ; // USBネイティブ UARTの場合は、シリアル通信が可能になるまで待つ
  }

  Serial.println("Goodnight moon!");

  // ソフトウェアシリアル通信を開始する
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}

void loop() {
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}
```

[PCIntVect](https://github.com/askn37/PCIntVect/) がインポートされているので、
以下のこともできる。

```c
// #include <pcintvect.h> の機能
// 他のポートの PCINT割込を使用開始
attachPCInterrupt(A0, togglePinVect);

volatile bool toggle = true;

void togglePinVect (void) {
  // bool edge = digitalRead(A0);
  toggle = ~toggle;
  digitalWrite(LED_BUILTIN, toggle);
}

// 割込を停止 - A0だけ停止、割込ベクタは解除しない
detachPCInterrupt(A0);

// 割込を停止 - A0を含むポートグループ全体を止め、割込ベクタも解除
detachPCInterrupt(A0, true);

```

## リファレンス

### bool SoftUART (int8\_t receivePin, int8\_t transmitPin, bool inverse\_logic = false)

コンストラクタでは、受信ピン、送信ピンを指定する。
省略可能な第3引数に true を指示すると、通信路のビット判定を負論理として動作する。

オリジナルでは、標準割込 attachInterrupt() が使える受信ピンを第1引数に指定した場合でも
PCINT が使える場合はそちらが強制的に使用されていたが、本モジュールでは
attachInterrupt() をより優先して使用する。
つまりその場合は PCINT を使用しない。

参考として、attachInterrupt() で使用可能なピンを以下に示す。
また PCINTをサポートしていない品種についても記す。

| 品種 | 標準割込ピン (INT0~) | PCINT0_vector (PCINT0-7) | PCINT1_vector (PCINT8-15) | PCINT2_vector (PCINT16-23) | PCINT3_vector (PCINT24-31) | ハードウェアサポート |
|---|---|---|---|---|---|---|
| ATmega 48 / 88 / 168 / 328    | 2, 3 | 8, 9, 10, 11, 12, 13, 20, 21 | 14(A0), 15(A1), 16(A2), 17(A3), 18(A4), 19(A5), 22 | 0, 1, 2, 3, 4, 5, 6, 7 | - | Arduino UNO / ProMini, MINICORE |
| ATmega 88PB / 168PB / 328PB   | 2, 3 | 8, 9, 10, 11, 12, 13, 20, 21 | 14(A0), 15(A1), 16(A2), 17(A3), 18(A4), 19(A5), 22 | 0, 1, 2, 3, 4, 5, 6, 7 | 23, 24, 25, 26 | MINICORE |
| ATmega 8                      | 2, 3 | - | - | - | - | MINICORE |
| ATmega 64 / 128               | 18, 19, 20, 21, 4, 5, 6, 7 | - | - | - | - | MEGACORE |
| ATmega 1281 / 2561            | 18, 19, 20, 21, 4, 5, 6, 7 | 8, 9, 10, 11, 12, 13, 14, 15 | - | - | - | MEGACORE |
| ATmega 640 / 1280 / 2560      | 21, 20, 19, 18, 3, 2, 71, 72 | 8, 9, 10, 11, 12, 13, 14, 15 | 0, 15, 14, 80, 81, 82, 83, 84 | 62(A8), 63(A9), 64(A10), 65(A11), 66(A12), 67(A13), 68(A14), 69(A15) | - | Arduino MEGA, MEGACORE (MEGA pinout) |
| ATmega 640 / 1280 / 2560      | 34, 35, 36, 37, 4, 5, 6, 7 | 15, 16, 17, 18, 19, 20, 21 | 0, 52, 53, 54, 55, 56, 57, 58 | 77(A8), 78(A9), 79(A10), 80(A11), 81(A12), 82(A13), 83(A14), 84(A15) | - |  MEGACORE (AVR pinout) |
| ATmega 164 / 324 / 644 / 1284 | 10, 11, 2   | 24(A0), 25(A1), 26(A2), 27(A3), 28(A4), 29(A5), 30(A6), 31(A7) | 0, 1, 2, 3, 4, 5, 6, 7 | 16, 17, 18, 19, 20, 21, 22, 23 | 8, 9, 10, 11, 12, 13, 14,  15 | MIGHTYCORE (STANDARD pinout) |
| ATmega 164 / 324 / 644 / 1284 | 2, 3, 6    | ※逆順 21(A7), 20(A6), 19(A5), 18(A4), 17(A3), 16(A2), 15(A1), 14(A0) | 4, 5, 6, 7, 10, 11, 12, 13 | 22, 23, 24, 25, 26, 27, 28, 29 | 0, 1, 2, 3, 30, 8, 9, 31 | MIGHTYCORE (BOBUINO pinout) |
| ATmega 8535 / 16 / 32         | 10, 11, 2  | - | - | - | - | MIGHTYCORE (STANDARD pinout) |
| ATmega 8535 / 16 / 32         | 2, 3, 6 | - | - | - | - | MIGHTYCORE (BOBUINO pinout) |
| ATmega 162                    | 10, 11, 32 | 24, 25, 26, 27, 28, 29, 30, 31 | 16, 17, 18, 19, 20, 21, 22, 23 | - | - | MAJORCORE |
| ATmega 8515                   | 10, 11, 32 | - | - | - | - | MAJORCORE |
| ATmega 32U2 / 32U4            | 3, 2, 1, 0 | 14, 13, 12, 11, 8, 9, 10 | - | - | - | Arduino Leonard / ProMicro |

- 参考： https://github.com/MCUdude

第2引数の送信ピンについては、digitalWrite() の使える全てのピンが指定できる。

ちなみに、標準割込が使えるピンは、以下のマクロと式で判定できる。

```c
#include <avr/interrupt.h>
if ( digitalPinToInterrupt( digitalPinNumber ) < EXTERNAL_NUM_INTERRUPTS ) {
  ; // Regular interrupt supported.
}
```

PCINTn_vector が使えるピンは、以下のマクロと式で判定できる。
```c
#include <avr/interrupt.h>
if ( digitalPinToPCICR( digitalPinNumber ) ) {
  ; // PCINT interrupt supported.
}
```

さらにどの PCINTn_vector ポートグループ に属するか知るには、以下のマクロが使用できる。
```c
#include <avr/interrupt.h>
uint8_t portGroup = digitalPinToPCICRbit( digitalPinNumber ); // PCINT0 - PCINT3
```

### void begin (long speed)

通信ボーレートを指定する。実用上の下限は 4800、上限は 38400、最大で 115200 である。

ソフトウェアシリアルはその実装の制約により、
1バイトぶんの送受信間は一切の他の割り込みを禁止する。
換言すれば、連続して連続したデータを受信する場合、
ストップビットを認識した直後の僅かな時間しか、
本来なすべき処理が実行できなくなってしまう。
極端な場合、他のタイマー動作進行や、ハードウェアシリアルの動作にも支障をきたす。

このため GPS(NMEA format) のように大量の高速データ受信が必要な場合など
受信バッファ（標準では64バイト）では捌ききれない用途には向いていないことに注意されたい。

### bool listen (void)

このオブジェクトでの受信を開始する。成功すれば真を返す。

複数のソフトウェアシリアルオブジェクトを使用する場合、
活性化できるのは、そのうちのいずれかひとつだけである。従って複数シリアル受信は、
このメソッドで逐次活性化するオブジェクトを切り替えなければならない。

切り替えなしで複数シリアルを同期的に、並列受信しなければならない場合には、
ハードウェアシリアルを使用するか、
[MultiUART](https://github.com/askn37/MultiUART/) を検討されたい。

### bool stopListening (void)
このオブジェクトでの受信を停止する。成功すれば真を返す。

listen() を実行すればその他のオブジェクトは自ずと停止するので使う場面は少ないが、
明示的に受信割込みを禁止して、MCU全体の処理速度を上げるのには役立つ。
特に諸々のタイマー動作進行が不正になるような場合は試してみると良い。

### bool isListening (void)

オブジェクトが活性化されているなら真を返す。

### bool overflow (void)

受信バッファ（標準では64バイト）が溢れているなら真を返す。
受信バッファが溢れている間の受信バイトは、捨てられる。

### int available (void)

受信バッファに保持されている受信データ数を返す。

### write/read/flush/print/println...

このモジュールは Streamクラスを継承しているので、
これらのメソッドは通常通り使用できる。

write() 等の送信動作については、実行、即、同期的に、送信データを送る。
従って送信バッファは有しておらず、flush() については何もしない空関数である。

### #define _SS_MAX_RX_BUFF 64

受信バッファサイズを保持するマクロ定数。標準は64。
この値を変更するには、ライブラリをインクルードする前に宣言されていなければならない。
指定する値は2の冪乗とすべきである。
指定した値の量の SRAM はそのまま専有されるので、256 が一般的な上限であろう。
ATmega1284 のような大容量 SRAM を有する品種では、1024 でも問題ないことはある。

----

## 既知の不具合／制約／課題

- 主要な AVR 以外はテストされていない。
- 古い Arduino IDE には対応しない。1.8.5で動作確認。少なくとも C++11 は使えなければならない。
- オリジナルの SoftwareSerial.h と共存できない。
同様に PCInt Vecotr を排他的に使用するライブラリとは共存できない。
- megaAVR-0系 tinyAVR-0/1/2系 では動作しない。
割込の基本構造が異なり旧来の PCInt は存在しない。
故に通常の SoftwareSerial で何ら問題はない。
- 英文マニュアルが未整備である。

## 改版履歴

- 0.1.1
  - megaAVR/tinyAVR系選択時には無効化されるようにした。

- 0.1

## 使用許諾

MIT

## 著作表示

朝日薫 / askn
(SenseWay Inc.)
Twitter: [@askn37](https://twitter.com/askn37)
GitHub: [https://github.com/askn37](https://github.com/askn37)
