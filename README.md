# Galaga Klonu - YZM104 Projesi
Hazırlayan: Taha Göktuğ Büyüksarı
Öğrenci No: 250229061
Bu proje, YZM104 - Programlama II dersi kapsamında geliştirilmiş, C dili ve SDL3 grafik kütüphanesi kullanılarak oluşturulmuş bir "Shoot 'em up" (düşman vurma) oyunudur.

## 🎮 Oyun Hakkında
Oyuncu, uzaylı düşman formasyonlarına karşı gemisini kontrol ederek hayatta kalmaya çalışır. Oyun; farklı düşman tipleri, güçlendirici (power-up) sistemi, boss savaşları ve dinamik zorluk seviyeleri içerir.

## 🚀 Kontroller
* **Hareket:** A (Sol), D (Sağ), W (Yukarı), S (Aşağı)
* **Ateş:** SPACE (Boşluk) tuşu
* **Özel Çoklu Ateş:** T tuşu
* **Yeniden Başla:** R tuşu (Oyun bittiğinde)
* **Çıkış:** ESC tuşu

## 🛠️ Teknik Özellikler
* **Dil:** C
* **Kütüphaneler:** * [SDL3](https://github.com/libsdl-org/SDL): Pencere yönetimi ve grafik işleme.
    * [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf): Yazı tipi (font) renderlama.
* **Algoritmalar:** * "Object Pooling" (Nesne Havuzlama) ile bellek yönetimi.
    * Bounding Box (AABB) çarpışma tespiti.
    * Dosya tabanlı yüksek skor (High Score) sistemi.

## 📋 Kurulum ve Derleme
Proje klasöründe terminali açıp aşağıdaki komutu kullanarak oyunu derleyebilirsiniz:

```bash
make
