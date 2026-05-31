#include <SDL3/SDL.h>         // Temel SDL3 kütüphanesi (Pencere, çizim, girdi vb. için)
#include <SDL3_ttf/SDL_ttf.h> // SDL3 Yazý Tipi (Font) kütüphanesi (Ekrana yazý yazdýrmak için)
#include <stdio.h>            // Standart giriþ/çýkýþ kütüphanesi (printf, file iþlemleri için)
#include <stdlib.h>           // Standart kütüphane (rastgele sayý üretimi malloc vb. için)
#include <stdbool.h>          // Mantýksal (doðru/yanlýþ) deðiþken türü için.
#include <time.h>             // Zaman kütüphanesi (rastgele sayýlarý zamana baðlamak için)


// --- OYUN AYARLARI VE SABÝTLERÝ ---


#define WIDTH 800           // Oyunun geniþliði.
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

typedef struct
{
    float x, y;                 // Yýldýzýn konumu
    float speedX, speedY;       // Yýldýzýn x ve y ekseninde ki hýzlarý
    float size;                 // Yýldýzýn boyutu piksel cinsinde
    SDL_Color color;            // Yýldýzýn rengi
} Star;





// Patlama Efekti Yapýsý
typedef struct
{
    float x, y;                 // Parçacýðýn konumu
    float speedX, speedY;       // Parçacýðýn  hýzý
    float life, maxLife;        // Parçacýðýn ekranda kalma süresi
    SDL_Color color;            // Parçacýðýn rengi
    bool active;                // Parçacýk aktif mi?
} Particle;



// Güçlendirici Yapýsý
typedef struct
{
    float x, y;                 // Güçlendiricinin konumu
    float speedY;               // Aþaðý doðru düþeceði için Y ekseninde ki hýzý
    int type;                   // Hangi güçlendirici?
    bool active;                // Ekranda aktif mi?
} PowerUp;




// Bilgi Yazýsýnýn Yapýsý (kalkan vb. için)
typedef struct
{
    float x, y;                 // Yazýnýn konumu
    char text[32];              // Görünecek yazý
    float life;                 // Ekranda görünme süresi
    SDL_Color color;            // Yazýnýn rengi
    bool active;                // Aktif mi?
} PopupText;



// Oyuncu Gemisinin yapýsý
typedef struct
{
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
typedef struct
{
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
typedef struct
{
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
int formationDirection = 1;



//---Yardımcı Fonksiyonlar---

int loadHighScore()
{
    FILE*file=fopen("highscore.txt","r");//En yüksek skorun kalıcı olması sağlandı.
    int hs=0;
    if(file)
    {
        fscanf(file,"%d",&hs);//highscore.txt dosyası yok ise en yüksek skor 0 dan başlar.
        fclose(file);
    }
    return hs;
}

void saveHighScore(int hs)
{

    FILE*file=fopen("highscore.txt","w");//highscore.txt içine yeni yüksek skoru yazar.
    if(file)
    {
        fprintf(file,"%d",hs);
        fclose(file);

    }
}

//---Mermi ateşleme---
void shootBullet(float x, float y, float w, float h, float speedX, float speedY, bool isEnemy, int damage)
{
    for (int i = 0; i < MAX_BULLETS; i++)  //define ile oluşturduğumuz 200 mermilik diziyi baştan sonra tarar
    {
        if (!bullets[i].active) //aktif olmayan mermi dizisini arar{
            bullets[i].x = x;
        bullets[i].y = y;
        bullets[i].width = w;
        bullets[i].height = h;//merminin özellikleri
        bullets[i].speedX = speedX;
        bullets[i].speedY = speedY;//Merminin X ve Y ekseninde ki hızları
        bullets[i].isEnemyBullet = isEnemy;
        bullets[i].damage = damage;
        bullets[i].active = true;//Merminin düşmanmı yoksa bizim gemimizden mi diye belirlenir ve hasar değeri yüklenir
        break;
    }
}




// --- EFEKT VE SİSTEM FONKSİYONLARI ---

// Arka plandaki yıldızların hızını ve konumunu rastgale belirler
void initStars()
{
    for (int i = 0; i < MAX_STARS; i++) //yıldızları teker teker ayarlar
    {
        stars[i].x = rand() % WIDTH;//yıldızların ekran üzeerindeki rastgale x konumu
        stars[i].y = rand() % HEIGHT;//yıldızların ekran üzeerindeki rastgale y konumu
        stars[i].speedX = -(float)(rand() % 15 + 5); // Önde ki eksi yıldızların sağdan dola kayacağını gösterir. Hızlarının değer aralığı= (5-20)
        stars[i].speedY = (float)(rand() % 40 + 10); // Aşağıdan yukarı doğru kayma hareketi yapar.Hızlarıının değer aralğı(10-50)
        stars[i].size = (float)(rand() % 2 + 1);     // Yıldızın büyüklüğünü belirler. Değer aralığı(1-3)
        int shade = rand() % 100 + 50;               // (50-150) arası değer belirler bunu alttaki ifade de renk tonu belirlemke için kullanacağız
        stars[i].color = (SDL_Color)
        {
            shade, shade, shade, 255
        };// kırmızı yeşil ve mavi yerine aynı değerler atandı ki gri renkte olup kullanıcının dikkati dağılmasın. 255 değeri saydamlıktır.
    }
}




// Belirtilen koordinatta parçacık saçılımı oluşturur
void spawnExplosion(float x, float y, SDL_Color color, int count)
{
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES && spawned < count; i++)
    {
        if (!particles[i].active)
        {
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
void spawnPopup(float x, float y, const char* text, SDL_Color color)
{
    for (int i = 0; i < MAX_POPUPS; i++)
    {
        if (!popups[i].active)
        {
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
void trySpawnPowerUp(float x, float y)
{
    if (rand() % 100 < 15)   // %15 Şans
    {
        for (int i = 0; i < MAX_POWERUPS; i++)
        {
            if (!powerups[i].active)
            {
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
void initEnemies(int currentLevel)
{
    formationX = 150.0f;//Başlangıc konumu
    formationY = 50.0f;//Başlangıc konumu
    formationSpeedX = 100.0f + (currentLevel * 10.0f);//Başlangıç hızı 100 her seviyede artar

    if (currentLevel % 5 == 0)  //Her 5 seviyede bir boss
    {

        // --- BOSS BÖLÜMÜ ---
        isBossLevel = true;
        enemies[0].offsetX = 0.0f;
        enemies[0].offsetY = 0.0f;
        enemies[0].width = 200.0f;
        enemies[0].height = 100.0f;
        enemies[0].x = WIDTH / 2.0f - enemies[0].width / 2.0f;
        enemies[0].y = formationY;
        enemies[0].isDiving = false;
        enemies[0].active = true;
        enemies[0].isBoss = true;
        enemies[0].hp = 100 * (currentLevel / 5);//can seviyeye göre ayarlanır
        enemies[0].attackCooldown = 2.0f;
        for (int i = 1; i < MAX_ENEMIES; i++) enemies[i].active = false;
        formationX = enemies[0].x;

    }
    else
    {

        // ---- NORMAL BÖLÜM- ---
        isBossLevel = false;
        int rows = 3;
        int cols = 6;//3x6 lık düşman bölgesi
        float spacingX = 80.0f;
        float spacingY = 60.0f;//aralarında 80 ve 60 piksellik boşluk bırakır

        int index = 0;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                if (index < MAX_ENEMIES)
                {
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
                    if (randType < 60)  //60 dan küçükse normal türden oluşur
                    {
                        enemies[index].type = Type_NORMAL;
                        enemies[index].hp = 1;
                    }
                    else if (randType < 80)    //60-80 arası ise hızlı türden oluşur
                    {
                        enemies[index].type = Type_FAST;
                        enemies[index].hp = 1;
                    }
                    else
                    {
                        enemies[index].type = Type_ARMORED;//Geri kalan için zırhlı düşman türünden oluşur
                        enemies[index].hp = 3; // Zırhlılar 3 vuruşta ölür!
                    }
                    index++;
                }
            }
        }
    }
}


// Oyuncunun hasar alma fonksiyonu
void takePlayerDamage(int damage)
{
    // Eğer oyuncunun kalkanı varsa veya dokunulmazsa hasar almaz
    if (player.invulnerabilityTimer <= 0.0f && player.shieldTimer <= 0.0f)
    {
        player.hp -= damage;
        player.invulnerabilityTimer = 3.0f;

        if (player.hp <= 0)
        {
            player.lives--;
            if (player.lives > 0)
            {
                player.hp = 100;
            }
            else
            {
                player.hp = 0;
                isGameOver = true;
            }
        }
    }
}






bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);//Hepsi doğru ise çarpışma vardır
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float x, float y, SDL_Color color)  //Ekrana yazı yazdırmak için fonksiyon
{
    if (!font) return;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, 0, color);
    if (surface)
    {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);//Yazıyı önce geçici şeklilde ram'e yazar.
        SDL_FRect destRect = { x, y, (float)surface->w, (float)surface->h };//Nerede çıkacağını ve büyüklüğünü gösterir
        SDL_RenderTexture(renderer, texture, NULL, &destRect);//Hazırlanan yazıyı frame ile eşleştirir
        SDL_DestroySurface(surface);//Bellek temizlemek için kullanılır
        SDL_DestroyTexture(texture);//Bellek temizlemek için kullanılır
    }
}


// --- ÇİZİM FONKSİYONLARI ---

void drawPlayerShip(SDL_Renderer* renderer, float x, float y, float w, float h)
{
    SDL_FRect core = { x + w * 0.4f, y, w * 0.2f, h };//Ana gövde
    SDL_FRect wings = { x, y + h * 0.5f, w, h * 0.2f };//kanatlar
    SDL_FRect leftGun = { x, y + h * 0.2f, w * 0.1f, h * 0.6f };//Sol silah
    SDL_FRect rightGun = { x + w * 0.9f, y + h * 0.2f, w * 0.1f, h * 0.6f };//Sağ silah
    SDL_RenderFillRect(renderer, &core);//Boyama aşaması
    SDL_RenderFillRect(renderer, &wings);//Boyama aşaması
    SDL_RenderFillRect(renderer, &leftGun);//Boyama aşaması
    SDL_RenderFillRect(renderer, &rightGun);//Boyama aşaması
}

// TİP 1: Normal Düşman (Kırmızı)
void drawNormalEnemy(SDL_Renderer* renderer, float x, float y, float w, float h)
{
    SDL_FRect brain = { x + w * 0.2f, y, w * 0.6f, h * 0.5f };//Ana gövde
    SDL_FRect wings = { x, y + h * 0.2f, w, h * 0.3f };//Kanatlar
    SDL_FRect leftLeg = { x + w * 0.2f, y + h * 0.5f, w * 0.2f, h * 0.4f };//Sağ silah
    SDL_FRect rightLeg = { x + w * 0.6f, y + h * 0.5f, w * 0.2f, h * 0.4f };//Sol silah
    SDL_RenderFillRect(renderer, &brain);
    SDL_RenderFillRect(renderer, &wings);//Boyama işlemi
    SDL_RenderFillRect(renderer, &leftLeg);
    SDL_RenderFillRect(renderer, &rightLeg);//Boyama işlemi
}

// TİP 2: Hızlı/Avcı Düşman (Yeşil)
void drawFastEnemy(SDL_Renderer* renderer, float x, float y, float w, float h)
{
    SDL_FRect core = { x + w * 0.4f, y, w * 0.2f, h }; //Ana gövde
    SDL_FRect wings = { x + w * 0.2f, y, w * 0.6f, h * 0.3f }; //Kanatlar
    SDL_RenderFillRect(renderer, &core);
    SDL_RenderFillRect(renderer, &wings);//Boyama aşaması
}

// TİP 3: Zırhlı üşman (Mavi)
void drawArmoredEnemy(SDL_Renderer* renderer, float x, float y, float w, float h)
{
    SDL_FRect core = { x, y, w, h * 0.8f }; //A na gövde
    SDL_FRect cannon = { x + w * 0.3f, y + h * 0.8f, w * 0.4f, h * 0.2f }; //Silahlık
    SDL_RenderFillRect(renderer, &core);
    SDL_RenderFillRect(renderer, &cannon);//Boyama aşamsı
}

void drawBossShip(SDL_Renderer* renderer, float x, float y, float w, float h)
{
    SDL_FRect core = { x + w * 0.3f, y + h * 0.2f, w * 0.4f, h * 0.6f };//Ana gövde
    SDL_FRect wings = { x, y, w, h * 0.3f };//Kanatlar
    SDL_FRect leftGun = { x + w * 0.1f, y + h * 0.3f, w * 0.15f, h * 0.7f };//Sol silah
    SDL_FRect rightGun = { x + w * 0.75f, y + h * 0.3f, w * 0.15f, h * 0.7f };//Sağ silah
    SDL_FRect eye = { x + w * 0.45f, y + h * 0.4f, w * 0.1f, h * 0.2f };//Göz

    SDL_SetRenderDrawColor(renderer, 200, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &wings);
    SDL_RenderFillRect(renderer, &leftGun);
    SDL_RenderFillRect(renderer, &rightGun);
    SDL_SetRenderDrawColor(renderer, 100, 0, 100, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &core);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &eye);
}












//-------ANA FONKSİYON-----


int main(int argc, char* argv[])
{

    srand((unsigned int)time(NULL));//Rastgelelik motor zamanı sabitleme.Eğer yapılmazsa oyun her açıldığında aynı şeyleri tekrarlar.

    if (!SDL_Init(SDL_INIT_VIDEO)) return -1;
    if (!TTF_Init()) return -1;

    SDL_Window* window = SDL_CreateWindow("Galaga Klonu", WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN);//Tam ekran açma
    if (!window) return -1;
    SDL_HideCursor();//Fare imblecini saklamak

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);//Pencere ile ekran kartı arasında bir köprü oluşturur.
    if (!renderer) return -1;
    SDL_SetRenderLogicalPresentation(renderer, WIDTH, HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);//Her ekranda tam ekran oynayabilmek için kullanılır

    TTF_Font* font = TTF_OpenFont("arial.ttf", 20.0f);//Windowstan aldığım yazı tipinin bouyutu <(20px)
    TTF_Font* fontSmall = TTF_OpenFont("arial.ttf", 12.0f);//Windowstan aldığım yazı tipinin bouyutu (12px)




    // --- BAŞLANGIÇ AYARLARI ---
    player.width = 40.0f;
    player.height = 30.0f;
    player.x = WIDTH / 2.0f - player.width / 2.0f;
    player.y = HEIGHT - 50.0f;//Tam ortaya hizalama
    player.speed = 300.0f;
    player.cooldown = 0.0f;
    player.lives = 2;
    player.hp = 100;
    player.ammo = 60;
    player.ammoTimer = 0.0f;
    player.invulnerabilityTimer = 0.0f;
    player.shieldTimer = 0.0f;
    player.rapidFireTimer = 0.0f;

    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;
    for (int i = 0; i < MAX_POWERUPS; i++) powerups[i].active = false;
    for (int i = 0; i < MAX_POPUPS; i++) popups[i].active = false;

    initStars(); // Yıldızları başlat
    initEnemies(level);//Level ekler
    highScore = loadHighScore();//High score varsa yazdırır

    bool running = true;
    SDL_Event event;//Klavyeden okuma yapar
    Uint64 lastTime = SDL_GetTicks();//Zamanı kaydeder ve ne kadar yavaş bir bilgisayar olsa bile donmadan oynatabilir



    // =           ANA OYUN DÖNGÜSÜ             =



    while (running)
    {
        Uint64 currentTime = SDL_GetTicks();//SDL motoru başladığından beri kaç milisaniye geçtiğini söyler
        float dt = (currentTime - lastTime) / 1000.0f;//Frameler arası süreyi belirler
        lastTime = currentTime;//Değer güncellemesi yapar



        while (SDL_PollEvent(&event))  //Tüm girişleri alır ve sırasıyla işler
        {
            if (event.type == SDL_EVENT_QUIT) running = false;//Çarpı ile kapatmak
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE) running = false;//ESC tuşu ile oyunu kapatmak

                // ÇOKLU ATIŞ (T)
                if (event.key.key == SDLK_T && !isGameOver)
                {
                    if (player.cooldown <= 0.0f && player.ammo >= 3)
                    {
                        float midX = player.x + player.width / 2.0f - 2.0f;//Midx merminin tam geminin ortasından çıkmasını sağlar
                        shootBullet(midX, player.y, 4.0f, 15.0f, 0.0f, -600.0f, false, 10);//Yukarı yönde ateşlenir
                        shootBullet(midX, player.y, 4.0f, 15.0f, -150.0f, -600.0f, false, 10);//Sola çapraz ateşlenir
                        shootBullet(midX, player.y, 4.0f, 15.0f, 150.0f, -600.0f, false, 10);//Sağa çapraz ateşlenir
                        player.cooldown = 0.6f;//0,6 saniyelik cooldown eklenir
                        player.ammo -= 3;//Mermi sayısı her defasında 3 azaltılır
                    }
                }

                if (event.key.key == SDLK_R && isGameOver)  //Güvenlik şartı
                {
                    isGameOver = false;
                    score = 0;
                    level = 1;
                    enemiesKilled = 0;//Tekrardan başlatıldığı için sıfırlar
                    player.lives = 2;
                    player.hp = 100;
                    player.ammo = 60;//Oyuncuyu başlangıç değerlerine ayarlar
                    player.ammoTimer = 0.0f;
                    player.invulnerabilityTimer = 0.0f;
                    player.shieldTimer = 0.0f;
                    player.rapidFireTimer = 0.0f;
                    player.x = WIDTH / 2.0f - player.width / 2.0f;
                    player.y = HEIGHT - 50.0f;
                    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;//Hafıza temizliği yapılır
                    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;
                    for (int i = 0; i < MAX_POWERUPS; i++) powerups[i].active = false;
                    initEnemies(level);//Düşmanları yeniden dizer
                }
            }
        }

        const bool* state = SDL_GetKeyboardState(NULL);//Kesintisiz iletişim sağlar

        if (!isGameOver)
        {
            //NORMAL ATEŞ ETME(space)
            if (state[SDL_SCANCODE_SPACE])  //Akıllı ateşleme sistemi eğer güçlendirici varsa 0,08 saniyede ateş eder yoksa 0,25 saniyede etmeye devam eder
            {
                // Eğer seri atış güçlendiricisi aktifse cooldown yokmuş gibi davranır
                float requiredCooldown = (player.rapidFireTimer > 0.0f) ? 0.08f : 0.25f;
                bool freeAmmo = (player.rapidFireTimer > 0.0f);

                if (player.cooldown <= 0.0f && (player.ammo >= 1 || freeAmmo))
                {
                    float midX = player.x + player.width / 2.0f - 2.0f;
                    shootBullet(midX, player.y, 4.0f, 15.0f, 0.0f, -600.0f, false, 10);
                    player.cooldown = requiredCooldown;
                    if (!freeAmmo) player.ammo -= 1; // Ücretsiz değilse mermi düş
                }
            }

            // OYUNCU HAREKETİ
            if (state[SDL_SCANCODE_A]) player.x -= player.speed * dt;//Sola hareket
            if (state[SDL_SCANCODE_D]) player.x += player.speed * dt;//Sağa hareket
            if (state[SDL_SCANCODE_W]) player.y -= player.speed * dt;//Yukarı hareket
            if (state[SDL_SCANCODE_S]) player.y += player.speed * dt;//Aşağı hareket

            if (player.x < 0) player.x = 0;//Oyuncunun ekrandan çıkması engellenir
            if (player.x > WIDTH - player.width) player.x = WIDTH - player.width;
            float maxY = HEIGHT - 50.0f;
            float minY = HEIGHT - 150.0f;
            if (player.y < minY) player.y = minY;
            if (player.y > maxY) player.y = maxY;



            // SAYAÇ GÜNCELLEMELERİ
            if (player.cooldown > 0.0f) player.cooldown -= dt;//Cooldown süresi
            if (player.invulnerabilityTimer > 0.0f) player.invulnerabilityTimer -= dt;//Ölümsüzlük süresi(hasar alındığında veya yeniden doğduğunda)
            if (player.shieldTimer > 0.0f) player.shieldTimer -= dt;//Kalkan süresi
            if (player.rapidFireTimer > 0.0f) player.rapidFireTimer -= dt;//Seri atış süresi

            player.ammoTimer += dt;
            if (player.ammoTimer >= 3.0f)
            {
                player.ammo += 1;    //3 saniyede bir mermi ekler
                player.ammoTimer -= 3.0f;
            }



            // ARKA PLAN ANİMASYONU
            for (int i = 0; i < MAX_STARS; i++)
            {
                stars[i].x += stars[i].speedX * dt;
                stars[i].y += stars[i].speedY * dt;
                //Ekrana geri yönlendirme
                if (stars[i].y > HEIGHT || stars[i].x < 0)
                {
                    stars[i].x = (float)(rand() % WIDTH + 100); // Sağdan daha çok gelir
                    stars[i].y = -(float)(rand() % 100);        // Üstten gelir
                }
            }


            // PATLAMA EFEKTİ
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (particles[i].active)
                {
                    particles[i].x += particles[i].speedX * dt;
                    particles[i].y += particles[i].speedY * dt;
                    particles[i].life -= dt;
                    if (particles[i].life <= 0.0f) particles[i].active = false; // Ömrü dolduğunda silinir
                }
            }



            // EKRANDAKİ HAREKETLİ YAZILAR
            for (int i = 0; i < MAX_POPUPS; i++)
            {
                if (popups[i].active)
                {
                    popups[i].y -= 30.0f * dt; //Yavaşça yukarı süzülür
                    popups[i].life -= dt;
                    if (popups[i].life <= 0.0f) popups[i].active = false;
                }
            }




            // GÜÇLENDİRİCİLER
            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                if (powerups[i].active)
                {
                    powerups[i].y += powerups[i].speedY * dt; // Aşağı doğru süzülür
                    if (powerups[i].y > HEIGHT) powerups[i].active = false; // Ekrandan çıktı

                    // Oyuncu güçlendiriciyi aldımı kontrol
                    if (checkCollision(player.x, player.y, player.width, player.height, powerups[i].x, powerups[i].y, 15.0f, 15.0f))
                    {
                        powerups[i].active = false;
                        score += 50; // Kutu alınca eklenen skor

                        // Türüne göre güçlendirici verip bilgisini ekranda göster
                        SDL_Color c;
                        if (powerups[i].type == Power_SHIELD)
                        {
                            player.shieldTimer = 10.0f; // 10 Saniye kalkan
                            c = (SDL_Color)
                            {
                                0, 150, 255, 255
                            };
                            spawnPopup(player.x, player.y - 20.0f, "+Kalkan! (10s)", c);
                        }
                        else if (powerups[i].type == Power_RAPIDFIRE)
                        {
                            player.rapidFireTimer = 5.0f; // 5 Saniye seri atış
                            c = (SDL_Color)
                            {
                                255, 200, 0, 255
                            };
                            spawnPopup(player.x, player.y - 20.0f, "+Seri Atis! (5s)", c);
                        }
                        else
                        {
                            player.ammo += 30; // +30 Mermi
                            c = (SDL_Color)
                            {
                                0, 255, 0, 255
                            };
                            spawnPopup(player.x, player.y - 20.0f, "+30 Cephane!", c);
                        }
                    }
                }
            }





            //       FORMASYON HAREKETİ
            formationX += formationSpeedX * formationDirection * dt;//Merkez noktasına göre ortak hız olarak ayarlandı
            bool hitEdge = false;
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active && !enemies[i].isDiving)
                {
                    float currentEnemyX = formationX + enemies[i].offsetX;
                    if (currentEnemyX <= 0 && formationDirection == -1)
                    {
                        hitEdge = true;
                        break;
                    }
                    if (currentEnemyX + enemies[i].width >= WIDTH && formationDirection == 1)
                    {
                        hitEdge = true;    //Tek bir düşman kenara çarparsa tüm filo başlangıç konumuna gider
                        break;
                    }
                }
            }
            if (hitEdge) formationDirection *= -1;//Duvara çarparsa yön değişir

            int activeEnemiesCount = 0;
            int divingCount = 0;
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {
                    activeEnemiesCount++;
                    if (enemies[i].isDiving) divingCount++;
                }
            }





            //  DÜŞMAN GÜNCELLEMELERİ
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {

                    if (!enemies[i].isDiving)
                    {
                        enemies[i].x = formationX + enemies[i].offsetX;//Düşman grubunu hizalar
                        enemies[i].y = formationY + enemies[i].offsetY;//Düşman grubunu hizalar

                        // Zırhlı düşmanlar dalış yapmazlar. Hızlı olanlar daha sık dalar.
                        int diveChance = (enemies[i].type == Type_FAST) ? 3 : (enemies[i].type == Type_ARMORED) ? 0 : 1;//Hızlı düşman için 3, normal için 2,zırhlı için 0 olarak ayarlandı
                        if (!enemies[i].isBoss && divingCount < 5 && rand() % 1000 < (diveChance + level))  //Dalışa geçme şartları
                        {
                            enemies[i].isDiving = true;
                            divingCount++;
                        }
                    }
                    else
                    {
                        // Türe göre dalış hızı ayarı
                        float diveSpeedMult = (enemies[i].type == Type_FAST) ? 1.8f : (enemies[i].type == Type_ARMORED) ? 0.6f : 1.0f;
                        enemies[i].y += (150.0f + level * 10.0f) * diveSpeedMult * dt;

                        if (enemies[i].y > HEIGHT)
                        {
                            enemies[i].isDiving = false;
                            enemies[i].y = formationY + enemies[i].offsetY;
                        }
                    }
                }



                // Düşman Atış Mekanizması
                if (enemies[i].isBoss)
                {
                    enemies[i].attackCooldown -= dt;
                    if (enemies[i].attackCooldown <= 0.0f)
                    {
                        int bossDmg = 30 + (level * 3);//Hasar sabit kalmaz artar
                        float centerX = enemies[i].x + enemies[i].width / 2.0f;
                        float bY = enemies[i].y + enemies[i].height;
                        int attackType = rand() % 100;

                        if (attackType < 25)  //%25 ihtimalle 3 mermi fırlatır
                        {
                            shootBullet(centerX - 3.0f, bY, 6.0f, 15.0f, 0.0f, 400.0f, true, bossDmg / 2);
                            shootBullet(centerX - 3.0f, bY, 6.0f, 15.0f, -60.0f, 400.0f, true, bossDmg / 2);
                            shootBullet(centerX - 3.0f, bY, 6.0f, 15.0f, 60.0f, 400.0f, true, bossDmg / 2);
                        }
                        else
                        {
                            shootBullet(centerX - 12.0f, bY, 24.0f, 45.0f, 0.0f, 400.0f, true, bossDmg);//%75 ihtimalle tek mermi fırlatır
                        }

                        enemies[i].attackCooldown = 1.5f + (rand() % 100) / 100.0f - (level * 0.02f);//Giderek boss leveli zorlaşır
                        if(enemies[i].attackCooldown < 0.5f) enemies[i].attackCooldown = 0.5f;
                    }
                }
                else
                {
                    // Normal-Zırhlı düşmanlar ateş eder Zırhlılar daha az ama güçlü atar
                    if (rand() % 4000 < (1 + level))  //Level arttıkça ateş etme sıklığı artar
                    {
                        int enemyDmg = 10 + (level * 2);//Level arttıkça hasar artar
                        if (enemies[i].type == Type_ARMORED) enemyDmg *= 2; // Zırhlı 2 kat hasar vurur
                        if (enemies[i].type == Type_FAST) enemyDmg /= 2;    // Hızlı yarım hasar vurur

                        shootBullet(enemies[i].x + enemies[i].width / 2.0f - 3.0f, enemies[i].y + enemies[i].height,
                                    6.0f, 15.0f, 0.0f, 300.0f + (level * 10.0f), true, enemyDmg);//Mermi hızı level arttıkça artar
                    }
                }




                //  HİTBOX AŞAMASI
                float px = player.x + player.width * 0.3f;
                float py = player.y + player.height * 0.3f;//Sağdan ve soldan %30 arlık boşluk bıraktım
                float pw = player.width * 0.4f;
                float ph = player.height * 0.6f;//Genişliği %40 olarak ayarlandı Yüksekliği %60 olarak ayarlandı
                float ex = enemies[i].x + enemies[i].width * 0.1f;
                float ey = enemies[i].y + enemies[i].height * 0.1f;//Düşman hitboxı %10 u kadar ayarlandı
                float ew = enemies[i].width * 0.8f;
                float eh = enemies[i].height * 0.8f;

                if (checkCollision(px, py, pw, ph, ex, ey, ew, eh))
                {
                    if (!enemies[i].isBoss) enemies[i].isDiving = false;
                    // ÇARPMA HASARI TÜRE GÖRE
                    int crashDmg = 25;
                    if (enemies[i].isBoss) crashDmg = 50;
                    if (enemies[i].type == Type_ARMORED) crashDmg = 40;
                    if (enemies[i].type == Type_FAST) crashDmg = 10;
                    takePlayerDamage(crashDmg);
                }
            }
        }


        int activeEnemiesCount = 0;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i].active)
            {
                activeEnemiesCount++;
            }
        }


        if (activeEnemiesCount == 0)
        {
            score += 500 * level;
            level++;//level arttıkça 500 puan ekler
            initEnemies(level);
        }





        //  MERMİ GÜNCELLEMELERİ VE VURDU MU DİYE KONTROL ETME
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].active)
            {
                bullets[i].x += bullets[i].speedX * dt;//X ekseninde zamana göre ilerler
                bullets[i].y += bullets[i].speedY * dt;//Y ekseninde zamana göre ilerler

                if (bullets[i].y < 0 || bullets[i].y > HEIGHT || bullets[i].x < 0 || bullets[i].x > WIDTH)  //Mermi ekrandan çıkmışsa false yapar ve siler
                {
                    bullets[i].active = false;
                    continue;//İşlemciyi gereksiz yere yormaz
                }

                if (!bullets[i].isEnemyBullet)
                {
                    for (int j = 0; j < MAX_ENEMIES; j++)  //Attığımız mermileri düşmanlara tek tek değdi mi diye kontrol eder
                    {
                        float ex = enemies[j].x + enemies[j].width * 0.1f;
                        float ey = enemies[j].y + enemies[j].height * 0.1f;//Düşmanların hitboxları daraltılmıştır
                        float ew = enemies[j].width * 0.8f;
                        float eh = enemies[j].height * 0.8f;

                        if (enemies[j].active && checkCollision(bullets[i].x, bullets[i].y, bullets[i].width, bullets[i].height, ex, ey, ew, eh))
                        {
                            bullets[i].active = false;
                            enemies[j].hp -= bullets[i].damage;//Düşmanın canından merminin hasarı çıkartılır



                            // Mermi vurduğunda kıvılcım efekti
                            SDL_Color sparkColor = {255, 255, 100, 255};
                            spawnExplosion(bullets[i].x, bullets[i].y, sparkColor, 3);//3 parçacık gösterir hasar verdiği için

                            if (enemies[j].hp <= 0)
                            {
                                enemies[j].active = false;
                                enemiesKilled++;

                                // DÜŞMAN ÖLDÜ - BÜYÜK PATLAMA EFEKTİ
                                SDL_Color expColor = enemies[j].isBoss ? (SDL_Color)
                                {
                                    200, 0, 255, 255
} :
                                (SDL_Color)
                                {
                                    255, 100, 0, 255
                                };//Boss ise mor diğerlerinde turuncu patlama olur
                                spawnExplosion(enemies[j].x + enemies[j].width / 2.0f, enemies[j].y + enemies[j].height / 2.0f, expColor, 20);//20 parçacıklı bir patlama gösterir

                                // ÖLEN DÜŞMANDAN KUTU DÜŞME İHTİMALİ
                                trySpawnPowerUp(enemies[j].x + enemies[j].width / 2.0f, enemies[j].y);//Yukarıda %15 ihtimalle kutu düşeceğini ayarlamıştık şansa bağlı olarak düşer

                                if (enemies[j].isBoss)
                                {
                                    score += 5000;
                                    player.ammo += 50;
                                    player.lives += 1;
                                    spawnPopup(WIDTH / 2.0f - 50.0f, HEIGHT / 2.0f, "BOSS YOK EDILDI! +1 CAN", (SDL_Color)
                                    {
                                        255, 255, 0, 255
                                    });
                                }
                                else
                                {
                                    score += 100;
                                    if(enemies[j].type == Type_ARMORED) score += 50; // Zırhlı kesmek zor olduğu için fazladan puan verir
                                }
                            }
                            break;
                        }
                    }
                }
                else
                {
                    float px = player.x + player.width * 0.3f;
                    float py = player.y + player.height * 0.3f;//Düşman mermisinin çarpıp çarpmadığına bakılır
                    float pw = player.width * 0.4f;
                    float ph = player.height * 0.6f;

                    if (checkCollision(bullets[i].x, bullets[i].y, bullets[i].width, bullets[i].height, px, py, pw, ph))
                    {
                        bullets[i].active = false;
                        takePlayerDamage(bullets[i].damage);//Eğer varsa sağlık azalır
                    }
                }
            }
        }

        if (isGameOver && score > highScore)
        {
            highScore = score;    //Skor kaydeder
            saveHighScore(highScore);
        }







        //////////////////////////////////////////////
        // =           EKRAN ÇİZİM AŞAMASI          =
        //////////////////////////////////////////////

        SDL_SetRenderDrawColor(renderer, 10, 10, 20, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);//Tüm ekranı boyar ve temizlemiş olur

        // 1) ARKA PLAN YILDIZ ÇİZİMİ
        for (int i = 0; i < MAX_STARS; i++)
        {
            SDL_SetRenderDrawColor(renderer, stars[i].color.r, stars[i].color.g, stars[i].color.b, stars[i].color.a);//Gri tonu atanır
            SDL_FRect sRect = { stars[i].x, stars[i].y, stars[i].size, stars[i].size };//Yıldıza kalıp çıkartır
            SDL_RenderFillRect(renderer, &sRect);//İçini boyar ve ekranda görünür
        }

        if (!isGameOver)
        {
            // 2) OYUNCU ÇİZİMİ
            if (player.invulnerabilityTimer <= 0.0f || (int)(player.invulnerabilityTimer * 8) % 2 == 0)  //Hasar almaz olduğu zaman yanıp söner
            {
                int r = 255 - (player.hp * 2.5f);
                if(r<0) r=0;
                if(r>255) r=255;//Gemi tam canlıyken açık mavi renktedir sağlık azaldıkça kırmızı mor renklere dönerek ölüyorsun izlemini yaratır
                int g = player.hp * 2.0f;
                if(g>200) g=200;
                SDL_SetRenderDrawColor(renderer, r, g, 255, SDL_ALPHA_OPAQUE);

                drawPlayerShip(renderer, player.x, player.y, player.width, player.height);//Geminin üzerine işlenir boyanır


                //  Kalkan Aktifse Etrafına Mavi Kare Çiz
                if (player.shieldTimer > 0.0f)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 150, 255, SDL_ALPHA_OPAQUE);//Koyu mavi bir renge ayarlandı
                    SDL_FRect sRect = { player.x - 5.0f, player.y - 5.0f, player.width + 10.0f, player.height + 10.0f };//Gemiyle arasına boşluk bırakır
                    // Retro bir his vermesi için FRect kullanıldı


                    // Retro Kalkan Efekti
                    SDL_FRect topB = { sRect.x, sRect.y, sRect.w, 2.0f };//Üst duvar
                    SDL_FRect botB = { sRect.x, sRect.y + sRect.h, sRect.w, 2.0f };//Alt duvar
                    SDL_FRect leftB = { sRect.x, sRect.y, 2.0f, sRect.h };//Sol duvar
                    SDL_FRect rightB = { sRect.x + sRect.w, sRect.y, 2.0f, sRect.h + 2.0f };//Sağ duvar
                    SDL_RenderFillRect(renderer, &topB);
                    SDL_RenderFillRect(renderer, &botB);//Hepsi aynı renge boyanır
                    SDL_RenderFillRect(renderer, &leftB);
                    SDL_RenderFillRect(renderer, &rightB);
                }



                // Sağlık Barı
                SDL_SetRenderDrawColor(renderer, 200, 0, 0, SDL_ALPHA_OPAQUE);//Kırmızı renk ayarlandı
                SDL_FRect bgBar = { player.x, player.y - 12.0f, player.width, 5.0f };//Gemiye göre konumu
                SDL_RenderFillRect(renderer, &bgBar);

                SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);//Yeşil renk ayarlandı
                float hpPercent = player.hp / 100.0f;
                if (hpPercent < 0.0f) hpPercent = 0.0f;//Yüzde hesabı yapılıyor
                SDL_FRect fgBar = { player.x, player.y - 12.0f, player.width * hpPercent, 5.0f };//Gemiye göre konumu
                SDL_RenderFillRect(renderer, &fgBar);

                char hpBarText[10];
                sprintf(hpBarText, "%d", player.hp);//Sayıyı yazıya çevirmemize olanak sağlar
                SDL_Color greenColor = { 0, 255, 0, 255 };
                renderText(renderer, fontSmall, hpBarText, player.x + player.width / 2.0f - 10.0f, player.y - 28.0f, greenColor);//Barın tam ortasına hizalanmasını sağlar
            }

            // 3) GÜÇLENDİRİCİ KUTULARI ÇİZİMİ
            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                if (powerups[i].active)
                {
                    char pText[2];//2 olmasının nedeni \0 eklenmesidir
                    if (powerups[i].type == Power_SHIELD)
                    {
                        SDL_SetRenderDrawColor(renderer, 0, 150, 255, SDL_ALPHA_OPAQUE); // Kalkan (Mavi)
                        sprintf(pText, "K");//Kalkan için K harfi ve mavi renk kullanıldı
                    }
                    else if (powerups[i].type == Power_RAPIDFIRE)
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 200, 0, SDL_ALPHA_OPAQUE); // Seri Atış (Sarı)
                        sprintf(pText, "S");//Seri atış için S harfi ve turuncu renk kullanıldı
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);   // Mermi (Yeşil)
                        sprintf(pText, "M");//Ekstra mermi içim M harf, ve yeşil renk kullanıldı
                    }

                    SDL_FRect pRect = { powerups[i].x, powerups[i].y, 15.0f, 15.0f };//15x15 lik kutu oluşturuklup içine güçlendiricinin harfi yazılır
                    SDL_RenderFillRect(renderer, &pRect);

                    // Kutunun üstüne harfini yaz
                    SDL_Color black = {0,0,0,255};
                    renderText(renderer, fontSmall, pText, powerups[i].x + 3.0f, powerups[i].y, black);
                }
            }

            // 4) DÜŞMAN ÇİZİMLERİ
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {
                    if (enemies[i].isBoss)
                    {
                        drawBossShip(renderer, enemies[i].x, enemies[i].y, enemies[i].width, enemies[i].height);//Daha önceden yazdığım fonksiyonu çağırır
                        char bossHpText[30];
                        sprintf(bossHpText, "BOSS HP: %d", enemies[i].hp);//Sprintf ile can değeri metne çevrilip kırmızı renkte yazılır
                        SDL_Color bColor = {255, 0, 0, 255};
                        renderText(renderer, font, bossHpText, enemies[i].x + 30.0f, enemies[i].y - 30.0f, bColor);
                    }
                    else
                    {
                        if (enemies[i].type == Type_FAST)
                        {
                            SDL_SetRenderDrawColor(renderer, 50, 255, 50, SDL_ALPHA_OPAQUE); // Hızlı olan düşmanlar yeşil olarak ayarlandı
                            drawFastEnemy(renderer, enemies[i].x, enemies[i].y, enemies[i].width, enemies[i].height);
                        }
                        else if (enemies[i].type == Type_ARMORED)
                        {
                            SDL_SetRenderDrawColor(renderer, 50, 150, 255, SDL_ALPHA_OPAQUE); // Zırhlı olan düşmanlar mavi olarak ayarlanadı
                            drawArmoredEnemy(renderer, enemies[i].x, enemies[i].y, enemies[i].width, enemies[i].height);
                        }
                        else
                        {
                            SDL_SetRenderDrawColor(renderer, 255, 50, 50, SDL_ALPHA_OPAQUE); // Normal düşmanlar kırmızı ayarlandı
                            drawNormalEnemy(renderer, enemies[i].x, enemies[i].y, enemies[i].width, enemies[i].height);
                        }
                    }
                }
            }



            // 5) MERMİ ÇİZİMİ
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullets[i].active)
                {
                    if (bullets[i].isEnemyBullet)
                    {
                        if (bullets[i].height > 40.0f) SDL_SetRenderDrawColor(renderer, 255, 100, 0, SDL_ALPHA_OPAQUE);//Turuncu boss için
                        else SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);//Kırmızı normal düşman için
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);//Sarı oyuncu için
                    }
                    SDL_FRect bRect = { bullets[i].x, bullets[i].y, bullets[i].width, bullets[i].height };
                    SDL_RenderFillRect(renderer, &bRect);
                }
            }

            // 6) PATLAMA EFEKTİ ÇİZİMİ
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (particles[i].active)
                {
                    // Ömrü azaldıkça boyutu ufalır
                    float size = 4.0f * (particles[i].life / particles[i].maxLife);//4x4 lük olarak başlarken zamanla 3x3,2x2 diye giderek azalır
                    if (size < 1.0f) size = 1.0f; // 1 pikselin altındaysa ekrana çizilmez

                    SDL_SetRenderDrawColor(renderer, particles[i].color.r, particles[i].color.g, particles[i].color.b, SDL_ALPHA_OPAQUE);
                    SDL_FRect pRect = { particles[i].x, particles[i].y, size, size };
                    SDL_RenderFillRect(renderer, &pRect);
                }
            }


            // 7) BİLGİLENDİRME YAZILARININ ÇİZİMLERİ
            for (int i = 0; i < MAX_POPUPS; i++)
            {
                if (popups[i].active)
                {
                    renderText(renderer, fontSmall, popups[i].text, popups[i].x, popups[i].y, popups[i].color);//Fonksiyonu çağırır ve fontsmall ile 12 piksellik fontla yazar.
                }
            }

            // 8) ARAYÜZ ÇİZİMİ
            char scoreText[150];
            char infoText[150];
            sprintf(scoreText, "Skor: %d   Level: %d   Can: %d   HP: %d   En Yuksek: %d", score, level, player.lives, player.hp, highScore);

            // Eğer güçlendirici aktifse yazıda göster
            char activePower[50] = "";//Kalan süreyi yazar
            if (player.shieldTimer > 0) sprintf(activePower, " [KALKAN: %.1fs]", player.shieldTimer);
            else if (player.rapidFireTimer > 0) sprintf(activePower, " [SERI ATIS: %.1fs]", player.rapidFireTimer);//Geri kalan süreyi ondalıklı gösterir

            sprintf(infoText, "Mermi: %d   Vurulan: %d %s", player.ammo, enemiesKilled, activePower);

            SDL_Color textColor = { 255, 255, 255, 255 };
            renderText(renderer, font, scoreText, 10.0f, 10.0f, textColor);
            renderText(renderer, font, infoText, 10.0f, 35.0f, textColor);
        }
        else
        {
            SDL_Color redColor = { 255, 50, 50, 255 };
            renderText(renderer, font, "OYUN BITTI! YENIDEN BASLAMAK ICIN 'R' TUSUNA BASIN.", 100.0f, HEIGHT / 2.0f, redColor);
            renderText(renderer, font, "CIKMAK ICIN 'ESC' TUSUNA BASIN.", 100.0f, HEIGHT / 2.0f + 40.0f, redColor);
        }

        SDL_RenderPresent(renderer); // Oyunu oynanabilen tutan koddur

    }

    //  TEMİZLİK
    if (font) TTF_CloseFont(font);
    if (fontSmall) TTF_CloseFont(fontSmall);
    SDL_ShowCursor();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();



    return 0;
}





































































































































