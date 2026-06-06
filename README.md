# Rangkuman Perubahan Project

Tanggal sesi: 27 Mei 2026

Project ini adalah game 3D OpenGL/GLUT berbasis bola yang bergerak di arena bertingkat, mengumpulkan star, menghindari lubang, dan mencapai bendera finish.

## Perubahan Gameplay dan Arena

1. Menambahkan perluasan arena setelah arena 6.
   - Ditambahkan bridge dari arena 6 ke arena 7.
   - Ditambahkan arena 7 sebagai area transit.
   - Ditambahkan jembatan sempit berkelok dari arena 7 menuju arena 8.
   - Arena 8 dijadikan finish arena baru.
   - Posisi bendera finish dipindahkan ke arena 8.

2. Mengatur ulang posisi arena 7 dan arena 8.
   - Bridge dari arena 6 ke arena 7 dipanjangkan.
   - Arena 7 dan seluruh jalur setelahnya ikut maju ke depan.
   - Arena 8 juga digeser lebih maju agar posisinya lebih sesuai dengan layout yang diinginkan.

3. Menambahkan barrier di arena 3.
   - Ditambahkan barrier panjang di sisi kiri lubang arena 3.
   - Ditambahkan barrier pendek sebagai pengarah dari pintu masuk arena 3.
   - Tujuannya agar pemain harus memutari lubang, bukan langsung melewati sisi kiri.

## Perubahan Physics dan Collision

1. Mengubah logika jatuh dari arena.
   - Jika bola keluar dari arena/lubang sampai tidak ada ground valid di bawahnya, status jatuh dikunci.
   - Setelah status jatuh terkunci, bola tidak bisa snap naik kembali ke platform walaupun posisi X/Z kembali ke atas arena.
   - Bola akan terus jatuh sampai mencapai batas `FLOOR_DEATH`, lalu game menjadi `GAMEOVER`.

2. Menambahkan collision solid untuk arena dan bridge.
   - Arena dan bridge tidak lagi hanya dianggap sebagai permukaan atas.
   - Sisi arena dan bridge sekarang memiliki collision volume.
   - Saat bola jatuh atau bergerak di samping platform, bola tidak langsung menembus sisi object.

3. Memperbaiki collision solid agar tidak mengganggu ramp.
   - Collision sisi platform dibatasi agar hanya aktif saat bola benar-benar berada di samping/bawah platform.
   - Area sambungan ramp dibuat sebagai connector/portal agar bola tetap bisa naik dari arena bawah ke arena atas.
   - Kasus yang diperbaiki:
     - Naik dari arena 2 ke arena 1.
     - Naik dari arena 4 ke arena 3.

## Perubahan Collectible dan Finish

1. Star spawn diperluas.
   - Star sekarang bisa muncul sampai arena 8.
   - Posisi spawn star disesuaikan dengan layout arena baru.

2. Win condition diperbarui.
   - Kemenangan sekarang terjadi saat bola menyentuh bendera di arena 8.
   - Posisi logika finish disinkronkan dengan posisi visual bendera.

## Perubahan Shadow

1. Sempat dibuat eksperimen shadow baru.
   - Shadow sempat dipisah ke file `Shadow.cpp` dan `Shadow.h`.
   - Shadow sempat dicoba agar berlaku untuk bola, star, barrier, dan bendera.

2. Perubahan shadow tersebut dibatalkan.
   - Sesuai permintaan, shadow dikembalikan ke state sebelumnya.
   - Project kembali memakai `drawBallShadow()` di `Ball.cpp`.
   - Shadow hanya kembali seperti implementasi awal pada bola.
   - File `Shadow.cpp` dan `Shadow.h` sudah dihapus dari source dan build.

## File yang Banyak Diubah

- `Common.h`
  - Menambah konstanta bridge dan arena baru.
  - Menambah ukuran/offset arena 8.

- `Arena.cpp`
  - Menggambar arena 7, bridge berkelok, arena 8, dan bendera finish baru.

- `Physics.cpp`
  - Menambah ground detection untuk arena/bridge baru.
  - Menambah collision solid untuk arena dan bridge.
  - Menambah pengecualian collision di area ramp connector.

- `Physics.h`
  - Menambah deklarasi fungsi collision solid dunia.

- `GameLogic.cpp`
  - Memperbarui posisi bendera finish.
  - Memperbarui logika jatuh agar tidak bisa kembali ke platform setelah keluar arena.
  - Memanggil collision solid setelah update posisi bola.

- `Barrier.cpp`
  - Menambah barrier di arena 3.
  - Menambah beberapa barrier tambahan di area akhir.

- `Star.cpp`
  - Menyesuaikan spawn star sampai arena baru.

- `Makefile.win`
  - Dibersihkan kembali agar tidak memakai file shadow eksperimen.

## Catatan Build

Project sudah beberapa kali berhasil di-build ulang memakai:

```bash
mingw32-make.exe -B -f Makefile.win
```

Jika build gagal dengan pesan `cannot open output file projecth.exe: Permission denied`, artinya game `projecth.exe` masih berjalan. Tutup game terlebih dahulu, lalu build ulang.

