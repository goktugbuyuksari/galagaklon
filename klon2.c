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


int score=0;
int highScore=0;
int enemiesKilled=0;
int level=1;
bool isBossLevel= false;
bool isGameOver= false;

float formationX=75.0f;
float formationY=25.0f;
float formationSpeedX=50.0f;



              //---Yardımcı Fonksiyonlar---

int loadHighsScore(){
FILE*file=fopen("highscore.txt","r");//En yüksek skorun kalıcı olması sağlandı.
int hs=0;
if(file){fscanf(file,"%d",&hs);//highscore.txt dosyası yok ise en yüksek skor 0 dan başlar.
fclose(file);
return hs;

        }
                     }

int saveHighScore(int hs){

FILE*file=fopen("highscore.txt","w");//highscore.txt içine yeni yüksek skoru yazar.
if(file) {fprintf(file,"%d",hs);
fclose(file);

         }
                        }

                //---Mermi ateşleme---
void shootBullet(float x, float y, float w, float h, float speedX, float speedY, bool isEnemy, int damage) {
    for (int i = 0; i < MAX_BULLETS; i++) {//define ile oluşturduğumuz 200 mermilik diziyi baştan sonra tarar
        if (!bullets[i].active) //aktif olmayan mermi dizisini arar{
            bullets[i].x = x; bullets[i].y = y; bullets[i].width = w; bullets[i].height = h;//merminin özellikleri
            bullets[i].speedX = speedX; bullets[i].speedY = speedY;//Merminin X ve Y ekseninde ki hızları
            bullets[i].isEnemyBullet = isEnemy; bullets[i].damage = damage; bullets[i].active = true;//Merminin düşmanmı yoksa bizim gemimizden mi diye belirlenir ve hasar değeri yüklenir
            break;
        }
    }




                                                 // --- EFEKT VE SİSTEM FONKSİYONLARI ---

    // Arka plandaki yıldızların hızını ve konumunu rastgale belirler
void initStars() {
    for (int i = 0; i < MAX_STARS; i++){//yıldızları teker teker ayarlar
        stars[i].x = rand() % WEIGHT;//yıldızların ekran üzeerindeki rastgale x konumu
        stars[i].y = rand() % HEIGHT;//yıldızların ekran üzeerindeki rastgale y konumu
        stars[i].speedX = -(float)(rand() % 15 + 5); // Önde ki eksi yıldızların sağdan dola kayacağını gösterir. Hızlarının değer aralığı= (5-20)
        stars[i].speedY = (float)(rand() % 40 + 10); // Aşağıdan yukarı doğru kayma hareketi yapar.Hızlarıının değer aralğı(10-50)
        stars[i].size = (float)(rand() % 2 + 1);     // Yıldızın büyüklüğünü belirler. Değer aralığı(1-3)
        int shade = rand() % 100 + 50;               // (50-150) arası değer belirler bunu alttaki ifade de renk tonu belirlemke için kullanacağız
        stars[i].color = (SDL_Color){shade, shade, shade, 255};// kırmızı yeşil ve mavi yerine aynı değerler atandı ki gri renkte olup kullanıcının dikkati dağılmasın. 255 değeri saydamlıktır.
         }
}




     // Belirtilen koordinatta parçacık saçılımı oluşturur
void spawnExplosion(float x, float y, SDL_Color color, int count) {
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES && spawned < count; i++) {
        if (!particles[i].active) {
            particles[i].x = x;   //Patlama merkezinin X konumu
            particles[i].y = y;   //Patlama merkezinin Y konumu
            // Etrafa rastgele yönlerde saçılması
            particles[i].speedX = (float)((rand() % 200) - 100);//Parçacığın X ekseninde ki hızı(-100-100)
            particles[i].speedY = (float)((rand() % 200) - 100);//Parçacığın Y ekseninde ki hızı(-100-100)
            particles[i].maxLife = (float)(rand() % 50 + 20) / 100.0f; // Parçacığın ömrü (0.2-0.7)saniye
            particles[i].life = particles[i].maxLife;
            particles[i].color = color;
            particles[i].active = true;//Kıvılcımı aktif eder
            spawned++;
        }
    }
}

      // Ekranda süzülerek kaybolan bilgilendirme yazılarını gösterir
void spawnPopup(float x, float y, const char* text, SDL_Color color) {
    for (int i = 0; i < MAX_POPUPS; i++) {
        if (!popups[i].active) {
            popups[i].x = x;//yazının çıkacağı x ekseni
            popups[i].y = y;//yazının çıkacağı y ekseni
            snprintf(popups[i].text, sizeof(popups[i].text), "%s", text);//snprintf taşma yapmadan oyunu çökertmeden güvenli bir şekilde girilen yazıyı kopyalamak için kullanılır
            popups[i].life = 1.5f; // Ekranda kalacağı süre
            popups[i].color = color; //yazının rengi
            popups[i].active = true;//aktif hale getirir
            break;
        }
    }
}

     // Ölen bir düşmandan %15 ihtimalle güçlendirici düşür
void trySpawnPowerUp(float x, float y) {
    if (rand() % 100 < 15) { // %15 Şans
        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (!powerups[i].active) {
                powerups[i].x = x;
                powerups[i].y = y;
                powerups[i].speedY = 100.0f; // Yavaşça aşağı düşer
                powerups[i].type = rand() % 3; // 3 güçlendiriciden birini rastgele seçer
                powerups[i].active = true;
                break;
            }
        }
    }
}

     // Seviyeye bağlı olarak düşmanları rastgele türlerde oluşturur
void initEnemies(int currentLevel) {
    formationX = 150.0f;//Başlangıc konumu
    formationY = 50.0f;//Başlangıc konumu
    formationSpeedX = 100.0f + (currentLevel * 10.0f);//Başlangıç hızı 100 her seviyede artar

    if (currentLevel % 5 == 0) {//Her 5 seviyede bir boss
        // --- BOSS BÖLÜMÜ ---
        isBossLevel = true;
        enemies[0].offsetX = 0.0f; enemies[0].offsetY = 0.0f;
        enemies[0].width = 200.0f; enemies[0].height = 100.0f;
        enemies[0].x = WEIGHT / 2.0f - enemies[0].width / 2.0f; enemies[0].y = formationY;
        enemies[0].isDiving = false; enemies[0].active = true; enemies[0].isBoss = true;
        enemies[0].hp = 100 * (currentLevel / 5);//can seviyeye göre ayarlanır
        enemies[0].attackCooldown = 2.0f;
        for (int i = 1; i < MAX_ENEMIES; i++) enemies[i].active = false;
        formationX = enemies[0].x;

     } else {
        // ---- NORMAL BÖLÜM- ---
        isBossLevel = false;
        int rows = 3; int cols = 6;//3x6 lık düşman bölgesi
        float spacingX = 80.0f; float spacingY = 60.0f;//aralarında 80 ve 60 piksellik boşluk bırakır

        int index = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (index < MAX_ENEMIES) {
                    enemies[index].offsetX = j * spacingX;//Düşman grubundan uzaklığı
                    enemies[index].offsetY = i * spacingY;//Düşman grubundan uzaklığı
                    enemies[index].x = formationX + enemies[index].offsetX;//Merkez noktası
                    enemies[index].y = formationY + enemies[index].offsetY;//Merkez noktası
                    enemies[index].width = 30.0f;//Düşman gemisinin hitboxını belirler
                    enemies[index].height = 20.0f;//Düşman gemisinin hitboxını belirler
                    enemies[index].isDiving = false;
                    enemies[index].active = true;
                    enemies[index].isBoss = false;

                    // Rastgele Düşman Tipi Belirleme (%60 Normal, %20 Hızlı, %20 Zırhlı)
                    int randType = rand() % 100;//*0-100 arası sayı belirler
                    if (randType < 60) {//60 dan küçükse normal türden oluşur
                        enemies[index].type = Type_NORMAL;
                        enemies[index].hp = 1;
                    } else if (randType < 80) {//60-80 arası ise hızlı türden oluşur
                        enemies[index].type = Type_FAST;
                        enemies[index].hp = 1;
                    } else {
                        enemies[index].type = Type_ARMORED;//Geri kalan için zırhlı düşman türünden oluşur
                        enemies[index].hp = 3; // Zırhlılar 3 vuruşta ölür!
                    }
                    index++;
                }
            }
        }
    }
}




























int main(int argc, char* argv[]) {





































    return 0;
}

































































































