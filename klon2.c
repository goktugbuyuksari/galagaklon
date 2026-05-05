#include <SDL3/SDL.h>         // Temel SDL3 kütüphanesi (Pencere, çizim, girdi vb. için)
#include <SDL3_ttf/SDL_ttf.h> // SDL3 Yazý Tipi (Font) kütüphanesi (Ekrana yazý yazdýrmak için)
#include <stdio.h>            // Standart giriþ/çýkýþ kütüphanesi (printf, file iþlemleri için)
#include <stdlib.h>           // Standart kütüphane (rastgele sayý üretimi malloc vb. için)
#include <stdbool.h>          // Mantýksal (doðru/yanlýþ) deðiþken türü için.
#include <time.h>             // Zaman kütüphanesi (rastgele sayýlarý zamana baðlamak için)


                                        // --- OYUN AYARLARI VE SABÝTLERÝ ---


#define WEIGHT 800           // Oyunun geniþliði.
#define HEIGHT 600          // Oyunun yüksekliði.
#define MAX_BULLETS 200     // Ekranda ayný anda bulunabilecek maksimum mermi sayýsý.
#define MAX_ENEMIES 18      // Ekranda bulunabilecek maksimum düþman sayýsý. (Burada 3 satýr x 6 sütun þeklindedir.)
#define MAX_STARS 100       // Arka plandaki maksimum kayan yýldýz sayýsý.
#define MAX_PARTICLES 300   // Patlama efektleri için maksimum parçacýk sayýsý.
#define MAX_POWERUPS 10     // Ekranda ayný anda bulanabilecek maksimum güçlendirici sayýsý.
#define MAX_POPUPS 10       // Ekranda çýkacak bilgilendirme yazýlarýnýn (kalkan sýnýrsýz mermi vb.) sayýsý.










                                    // --- DÜÞMAN VE GÜÇLENDÝRÝCÝ TÝPLERÝ  ------



  enum EnemyType { Type_NORMAL = 0, Type_FAST = 1, Type_ARMORED = 2 };            //normal--hýzlý ve zýrhlý olarak 3 düþman türü
  enum PowerUpType { Power_SHIELD = 0, Power_RAPIDFIRE = 1, Power_AMMO = 2 };  //kalkan--hýzlý veya seri atýþ ve ekstra mermi



                                            // ----Yapý Tanýmlamalarý----


           // Arkaplan Yýldýz Yapýsý

typedef struct {
    float x, y;                 // Yýldýzýn konumu
    float speedX, speedY;       // Yýldýzýn x ve y ekseninde ki hýzlarý
    float size;                 // Yýldýzýn boyutu piksel cinsinde
    SDL_Color color;            // Yýldýzýn rengi
} Star;





          // Patlama Efekti Yapýsý
typedef struct {
    float x, y;                 // Parçacýðýn konumu
    float speedX, speedY;       // Parçacýðýn  hýzý
    float life, maxLife;        // Parçacýðýn ekranda kalma süresi
    SDL_Color color;            // Parçacýðýn rengi
    bool active;                // Parçacýk aktif mi?
} Particle;



          // Güçlendirici Yapýsý
typedef struct {
    float x, y;                 // Güçlendiricinin konumu
    float speedY;               // Aþaðý doðru düþeceði için Y ekseninde ki hýzý
    int type;                   // Hangi güçlendirici?
    bool active;                // Ekranda aktif mi?
} PowerUp;




            // Bilgi Yazýsýnýn Yapýsý (kalkan vb. için)
typedef struct {
    float x, y;                 // Yazýnýn konumu
    char text[32];              // Görünecek yazý
    float life;                 // Ekranda görünme süresi
    SDL_Color color;            // Yazýnýn rengi
    bool active;                // Aktif mi?
} PopupText;



           // Oyuncu Gemisinin yapýsý
typedef struct {
    float x, y;           //Geminin anlýk koordinantlarý
    float width, height;  //Geminin geniþiði ve yüksekliði
    float speed;          // Geminin hýzý
    float cooldown;       //Bekleme süresi
    int lives;            //Can sayýsý
    int hp;               //Saðlýk (100-0)
    int ammo;             //Mermi sayýsý
    float ammoTimer;      //Zamanla mermi sayýsýnýn artmasý
    float invulnerabilityTimer;  //Hasar aldýktan sonra geminin belli bir süreliðine hasar almamasý




           // Güçlendirici Süreleri
    float shieldTimer;          // Kalkan süresi
    float rapidFireTimer;       // Seri veya hýzlý atýþ süresi
} Player;


           // Düþman ve Boss yapýsý
typedef struct {
    float x, y;                //Düþmanýn bulunduðu anlýk koordinantlar
    float offsetX, offsetY;    //Dalýþ yapmayan grup halinde bulunan düþmanýn sabit yeridir. Saða ve sola kayarken bu deðerlerden dolayý hizalarý bozulmaz
    float width, height;       //Düþman gemilerinin geniþliði ve yüksekliði
    bool isDiving;             //Düþman gemisinin dalýþa geçip geçmediðini belirlemek için
    bool active;               //Düþman gemisinin hayatta olup olmadýðýný belirler
    bool isBoss;               //Boss mu deðil mi diye kontrol eder
    int type;                   //Düþmanýn türünü belirler
    int hp;                     // Düþman gemilerinin saðlýðý
    float attackCooldown;        //Bossun tekrardan ateþ edebilmesi için gereken süre
} Enemy;


          // Mermi yapýsý
typedef struct {
    float x, y;             //Merminin anlýk konumu
    float width, height;    //Merminin geniþliði ve yüksekliði
    float speedX, speedY;   //Merminin X ve Y ekseninde ki hýzlarý
    bool isEnemyBullet;     //Düþman mermisi mi?
    int damage;             //Saðlýktan kaç götürecek?
    bool active;            //Mermi ekranda aktif mi?
} Bullet;

                                      // --- GLOBAL DEÐÝÞKENLER ---



Player player;
Enemy enemies[MAX_ENEMIES];
Bullet bullets[MAX_BULLETS];
Star stars[MAX_STARS];          // Yeni: Yýldýz dizisi
Particle particles[MAX_PARTICLES]; // Yeni: Partikül dizisi
PowerUp powerups[MAX_POWERUPS]; // Yeni: Güçlendirici dizisi
PopupText popups[MAX_POPUPS];   // Yeni: Yüzen yazý dizisi


































































































