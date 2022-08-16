#include "mbed.h"
#include "stm32f413h_discovery_ts.h"
#include <stdio.h>
#include <utility>
#include <stdlib.h>
#include <time.h>
TS_StateTypeDef TS_State = { 0 };

int frandom(int a, int b){
    // vraća random broj
    srand(time(NULL));
    return a + rand() % (b - a);
}
int frandom_niz(int niz[], int n){
    // vraća random element niza
    srand(time(NULL));
    return niz[rand() % n];
}
int frandom2(int a, int b, int c, int d){
    // vraća ili koordinte (a,b) ili koordinate (c,d)
    int rez=frandom(0,1);
    if(rez) return frandom(a,b);
    else return frandom(c,d);
}
void player(){
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(0,100,(uint8_t*)"HUMAN'S TURN",CENTER_MODE);
    
}
void comp(){
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(0,100,(uint8_t*)"COMPUTER'S TURN",CENTER_MODE);
    
}
void ships_() {
    // samo prikaz brodova
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(0,20,(uint8_t*)"CHOOSE SHIP",CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,40,(uint8_t*)"TO PLACE",CENTER_MODE);
    for(int i=80; i < 80+25*4 ; i+=25) {
        BSP_LCD_DrawRect(30, i, 25, 25);
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(30+1, i+1, 24, 23);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    }
    for(int i=80; i < 80+25*3 ; i+=25) {
        BSP_LCD_DrawRect(80, i, 25, 25);
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(80+1, i+1, 24, 23);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    }
    for(int i=80; i < 80+25*2 ; i+=25) {
        BSP_LCD_DrawRect(130, i, 25, 25);
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(130+1, i+1, 24, 23);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    }
    for(int i=80; i < 80+25*5 ; i+=25) {
        BSP_LCD_DrawRect(180, i, 25, 25);
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(180+1, i+1, 24, 23);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    }
    BSP_LCD_SetFont(&Font16);
}

bool stanje=0,jedan_igrac=0,dva_igraca=0;

int player1[8][8],player2[8][8],player1_strategy[8][8],player2_strategy[8][8];
bool ciji_je_red=0,isti_potez=0,strategy=0,kraj_partije=0;
int x_za_matricu=0,y_za_matricu=0;

char comp_matrix[8][8]={{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'}};
char player_matrix[8][8]={{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'},{'-','-','-','-','-','-','-','-'}};
int poeni1=0, poeni2=0;

void panel();//crta grid,exit i next 
void igrac_panel(); // ciji je red za strategiju/igru
void show_played();// prikazuje odigrani poteze/postavljene brodove za strategiju
void postavi_strategiju();
void start();

void uputa(){//about the game
    BSP_LCD_Init();
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(0,10,(uint8_t*)"ABOUT GAME",CENTER_MODE);

    int x=30, y=60;
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_FillRect(x, y, 26, 23);
    BSP_LCD_DisplayStringAt(x+30+4,y+5,(uint8_t*)"HIT ",LEFT_MODE);
         
    BSP_LCD_SetTextColor(LCD_COLOR_RED);            
    BSP_LCD_FillRect(x, y+30, 26, 23);
    BSP_LCD_DisplayStringAt(x+30+4,y+30+5,(uint8_t*)"SELECT",LEFT_MODE);
    
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);            
    BSP_LCD_FillRect(x, y+2*30, 26, 23);
    BSP_LCD_DisplayStringAt(x+30+4,y+2*30+5,(uint8_t*)"MISS",LEFT_MODE);
}
pair<int,int> touch(int pod=0){
    // vraća koordinate mjesta koje smo touch-ali
    // ukoliko se pozove sa vr. 1 vraća koordinate i ne boji mjesto koje smo touch-ali, inače vraća indekse reda i kolone
    int x=0, y=0;
    while(!x && !y){
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
                uint16_t x1 = TS_State.touchX[0];
                uint16_t y1 = TS_State.touchY[0];
                x=(x1-28)/26+1;
                y=(y1-40)/23+1;
                if(x<=8 && y<=8 && pod==0){
                BSP_LCD_SetTextColor(LCD_COLOR_RED);
                BSP_LCD_FillRect((x-1)*26+29, (y-1)*23+41, 25, 21); 
                BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                }
                if(pod==1)
                return pair<int,int> (y1,x1);
                else
                return pair<int,int> (y,x);
        }
        wait_ms(50);
    }
}
void postavi_strategiju(){
    igrac_panel();
    int ships[4]={1,1,1,1};
    
    while(ships[0] || ships[1] || ships[2] || ships[3]){
        ships_();
        BSP_LCD_SetFont(&Font20);
        if(!ciji_je_red) BSP_LCD_DisplayStringAt(0,40,(uint8_t*)" PLAYER 1 ",CENTER_MODE);
        if(ciji_je_red) BSP_LCD_DisplayStringAt(0,40,(uint8_t*)" PLAYER 2 ",CENTER_MODE);
        
        int m=0, n=0, a=0, b=0, size=0;
        
        //izaberi velicinu broda
        while(1){
            auto koord=touch(1);
            m=koord.first;
            a=koord.second;
            if(a >= 30 && a <=55 && m >= 80 && m <=180 && ships[2]) { size=4; ships[2]=0; break; }
            if(a >= 80 && a <=105 && m >= 80 && m <=155 && ships[1]) { size=3; ships[1]=0; break; }
            if(a >= 130 && a <=155 && m >= 80 && m <=130 && ships[0]) { size=2; ships[0]=0; break; }
            if(a >= 180 && a <=205 && m >= 80 && m <=205 && ships[3]) { size=5; ships[3]=0; break; }
        }
        wait(0.5);
        panel();
        show_played();
        
        int x1=0, y1=0; 
        
        while(!x1 || !y1 ){
            auto koord=touch(0);
            x1=koord.first;
            y1=koord.second;
            if(!ciji_je_red && player1_strategy[x1-1][y1-1] != 0) { x1=0; y1=0; wait(0.5); continue; }
            if(ciji_je_red && player2_strategy[x1-1][y1-1] != 0) { x1=0; y1=0; wait(0.5); continue; }
            else break;
        }
        wait(0.5);
        int x2=0, y2=0;
        while(!x2 || !y2 ){
            auto koord=touch(2);
            x2=koord.first;
            y2=koord.second;
            if(!ciji_je_red && (player1_strategy[x2-1][y2-1] != 0 || (abs(x1-x2)!=size-1 && abs(y1-y2)!=size-1) || (abs(x1-x2)==size-1 && y1!=y2) || (abs(y1-y2)==size-1 && x1!=x2))) { 
                x2=0; y2=0; 
                wait(0.5);
                continue;
            }
            if(ciji_je_red && (player2_strategy[x2-1][y2-1] != 0 || (abs(x1-x2)!=size-1 && abs(y1-y2)!=size-1) || (abs(x1-x2)==size-1 && y1!=y2) || (abs(y1-y2)==size-1 && x1!=x2))) { 
                x2=0; y2=0; 
                wait(0.5);
                continue;
            }
            else break;
        }
        x1--; x2--; y1--; y2--;
        if(x1==x2){
            int min, max;
            min=(y1<y2)?y1:y2;
            max=(y1>y2)?y1:y2;
            for(int j=min; j<=max; j++) { 
                if(!ciji_je_red) player1_strategy[x1][j]=1;
                if(ciji_je_red) player2_strategy[x1][j]=1;
                BSP_LCD_SetTextColor(LCD_COLOR_RED);
                BSP_LCD_FillRect((j)*26+29, (x1)*23+41, 25, 21);
                BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                wait_ms(50);
            }    
        } 
        else if(y1==y2){
            int min, max;
            min=(x1<x2)?x1:x2;
            max=(x1>x2)?x1:x2;
            for(int j=min; j<=max; j++) {
                if(!ciji_je_red) player1_strategy[j][y1]=1;
                if(ciji_je_red) player2_strategy[j][y1]=1;
                BSP_LCD_SetTextColor(LCD_COLOR_RED);
                BSP_LCD_FillRect((y1)*26+29, (j)*23+41, 25, 21);
                BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                if(j!=max)
                wait_ms(50);
            }    
        }
        wait_ms(50);
    }
    if (ciji_je_red) strategy=0;
    ciji_je_red=!ciji_je_red;
}
void panel(){
    BSP_LCD_Init ();
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font12);
    if(strategy==1 && !ciji_je_red){
        BSP_LCD_DisplayStringAt(0,5,(uint8_t*)"PLAYER'S 1 STRATEGY ",CENTER_MODE); 
    }
    if(strategy==1 && ciji_je_red){
        BSP_LCD_DisplayStringAt(0,5,(uint8_t*)"PLAYER'S 2 STRATEGY",CENTER_MODE);    
    }
    if(!strategy && ciji_je_red==0){
        BSP_LCD_DisplayStringAt(0,5,(uint8_t*)"PLAYER'S 1 TURN",CENTER_MODE);  
    }
    if(!strategy && ciji_je_red){
        BSP_LCD_DisplayStringAt(0,5,(uint8_t*)"PLAYER'S 2 TURN",CENTER_MODE);  
    }
    //EXIT
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
    BSP_LCD_FillRect(190, 225, 50, 13);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(200,226,(uint8_t*)"EXIT",LEFT_MODE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    //NEXT
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
    BSP_LCD_FillRect(2, 225, 50, 13);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(12,226,(uint8_t*)"NEXT",LEFT_MODE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    //grid
    BSP_LCD_SetFont(&Font16);
    int x=2, y=17;
    for(int i=0; i < 9; i++){
        for(int j=0; j < 9; j++){
            BSP_LCD_DrawRect(x,y,26,23);
            if(j==0 && i!=0){
                char red[2];
                sprintf(red,"%d",i);
                BSP_LCD_DisplayStringAt(x+8,y+7,(uint8_t*)red,LEFT_MODE);
            }
            if(j!=0 && i==0){
                char kolona[2];
                sprintf(kolona,"%c",64+j);
                BSP_LCD_DisplayStringAt(x+8,y+7,(uint8_t*)kolona,LEFT_MODE);
            }
            x+=26;
        }
        x=2;
        y+=23;
    }
}
void win(){
    if(jedan_igrac){
      if(poeni1>poeni2){
        BSP_LCD_Clear(LCD_COLOR_WHITE);
        BSP_LCD_SetFont(&Font20);
        BSP_LCD_DisplayStringAt(0,100,(uint8_t*)"YOU WIN!",CENTER_MODE);
       } else {
          BSP_LCD_Clear(LCD_COLOR_WHITE);
          BSP_LCD_SetFont(&Font20);
          BSP_LCD_DisplayStringAt(0,80,(uint8_t*)"YOU LOST!",CENTER_MODE);
          char score_s[20];
          BSP_LCD_SetFont(&Font16);
          sprintf(score_s, "Score: %d", poeni1);
          BSP_LCD_DisplayStringAt(0,150,(uint8_t*)score_s,CENTER_MODE);
        }
    }
    
   if(dva_igraca){
        int razlicito1=0,razlicito2=0;
        for(int i=0; i < 8; i++){
            for(int j=0; j < 8; j++){
                if(player1_strategy[i][ j] && player2[i][ j]!=player1_strategy[i][ j]){
                    razlicito1=1;
                    i=8;
                    j=8;
                }
            }
        }
        for(int i=0; i < 8; i++){
            for(int j=0; j < 8; j++){
                if(player2_strategy[i][ j] && player1[i][ j]!=player2_strategy[i][ j]){
                    razlicito2=1;
                    i=8;
                    j=8;
                }
            }
        }
        if(!razlicito1 || !razlicito2){
            kraj_partije=1;
            BSP_LCD_Init();
            BSP_LCD_Clear(LCD_COLOR_RED);
            BSP_LCD_SetFont(&Font24);
            BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
            if(!razlicito1) BSP_LCD_DisplayStringAt(0,65,(uint8_t*)"WIN PLAYER 2 ",CENTER_MODE);
            if(!razlicito2) BSP_LCD_DisplayStringAt(0,65,(uint8_t*)"WIN PLAYER 1 ",CENTER_MODE);
            wait(3);
        }
    }
}
void igrac_panel(){
    BSP_LCD_Init();
    BSP_LCD_Clear(LCD_COLOR_RED);
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    if(ciji_je_red==0 && strategy==1){
        BSP_LCD_DisplayStringAt(0,65,(uint8_t*)"PLAYER'S 1",CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,95,(uint8_t*)"STRATEGY ",CENTER_MODE);
        wait(2);
    }
    if(ciji_je_red && strategy==1){
        BSP_LCD_DisplayStringAt(0,65,(uint8_t*)"PLAYER'S 2",CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,95,(uint8_t*)"STRATEGY ",CENTER_MODE);
        wait(2);
    }
    if(ciji_je_red==0 && strategy==0){
        BSP_LCD_DisplayStringAt(0,65,(uint8_t*)"PLAYER'S 1",CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,95,(uint8_t*)"TURN",CENTER_MODE);
        wait(2);
    }
    if(ciji_je_red && strategy==0){
        BSP_LCD_DisplayStringAt(0,65,(uint8_t*)"PLAYER'S 2",CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,95,(uint8_t*)"TURN",CENTER_MODE);
        wait(2);
    }
}
void show_played(){
    if(dva_igraca && isti_potez==0){
        igrac_panel();
    }
    if(stanje==1 && strategy==0){
        panel();
        //oboji polja odgovarajuceg igraca
        for(int i=0; i < 8; i++){
            for(int j=0; j < 8; j++){
                if(!ciji_je_red && player1[i][ j]==1 ){
                    uint16_t x=j*26+3+26, y=i*23+23+18;
                    BSP_LCD_FillRect(x, y, 25, 21); 
                    //oboji u zeleno ako se tu nalazi brodic
                    if(player1[i][ j]==player2_strategy[i][ j]){
                        BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
                        BSP_LCD_FillRect(x, y, 25, 21);
                        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                    }
                }
                if(ciji_je_red && player2[i][j] ){
                    uint16_t x=j*26+3+26, y=i*23+23+18;
                    BSP_LCD_FillRect(x, y, 25, 21); 
                    //oboji u zeleno ako se tu nalazi brodic
                    if(player2[i][ j]==player1_strategy[i][ j]){
                        BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
                        BSP_LCD_FillRect(x, y, 25, 21);
                        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                    }
                } 
            }
        }
    }
    if(strategy){
        //oboji polja odgovarajuceg igraca
        for(int i=0; i < 8; i++){
            for(int j=0; j < 8; j++){
                //oboji u zeleno ako je postavljen brodic
                if(!ciji_je_red && player1_strategy[i][ j]==1 ){
                    uint16_t x=j*26+3+26, y=i*23+23+18;
                    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
                    BSP_LCD_FillRect(x, y, 25, 21);
                    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                }
                if(ciji_je_red && player2_strategy[i][j] ){
                     uint16_t x=j*26+3+26, y=i*23+23+18;
                    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
                    BSP_LCD_FillRect(x, y, 25, 21);
                    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                }
            }
        } 
    }
}    
// pozicioniranje brodova za računar
void pozicioniraj(int n){
    int x,y,i,j; 
    // odabir random orijent., 2 je stavljeno jer je inače veći procenat 0 nego 1
    int orijentacija=frandom(0,2);
    if(orijentacija){
        while(1){
            int greska=0;
            x=frandom(0,7);
            y=frandom(0,7);
            int m=0, i, j;
            for(i=y; i < 8; i++){
                // var. greska simbolizira da brod preklapa drugi
                if(n!=2 && comp_matrix[x][i]>='2' && comp_matrix[x][i]<='5') { greska=1; break;}
                // ukoliko smo pozicionirali brod
                m++;
                if(m==n) { y=i; break; }
                
            }
            if(greska) continue;
            if(i==8){
                for(i=y-1; i >= 0; i--){
                    if(n!=2 && comp_matrix[x][i]>='2' && comp_matrix[x][i]<='5') { greska=1; break;}
                    m++;
                    if(m==n) { y=i; break; }
                }
            }
            if(greska) continue;
            if(m==n && y+n<=8) break; 
        }
    } else {
        while(1){
            int greska=0;
            x=frandom(0,7);
            y=frandom(0,7);
            int m=0, i, j;
            for(i=y; i < 8; i++){
                if(n!=2 && comp_matrix[i][x]>='2' && comp_matrix[i][x]<='4') { greska=1; break;}
                m++;
                if(m==n) { y=i; break; }
                
            }
            if(greska) continue;
            if(i==8){
                for(i=y-1; i >= 0; i--){
                    if(n!=2 && comp_matrix[i][x]>='2' && comp_matrix[i][x]<='4') { greska=1; break;}
                    m++;
                    if(m==n) { y=i; break; }
                }
            }
            if(greska) continue;
            if(m==n && y+n<=8) break;
    }
    }
    // nakon sto smo odabrali pozicije i provjerili njihovu validnost, postavljamo brod
    if(orijentacija)
    for(i=y; i < y+n; i++) comp_matrix[x][i]='0'+n;
    else
    for(i=y; i < y+n; i++) comp_matrix[i][x]='0'+n;
}
void start_draw(char mat[8][8], int matr_ind=0){
    // ova funkcija se može pozvati sa matr_ind vrijednosti 1 za prikaz brodova igrača tokom igre -> tada su prikazani kao 'X', ne može zelena ispuna
    // može se pozvati sa matr_ind vrijednosti 2 za prikaz brodova igrača prilikom njihovog pozicioniranja -> tada su obojeni u zeleno 
    BSP_LCD_Init ();
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    
    //rezultat
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
   
    
    if(jedan_igrac){
               char score_[50]={};
               sprintf(score_, "Human: %d - Computer: %d", poeni1, poeni2);
               BSP_LCD_DisplayStringAt(0,5,(uint8_t*)score_,CENTER_MODE);
            }
    
    if(dva_igraca ){
         BSP_LCD_DisplayStringAt(0,5,(uint8_t*)"Igrac 1:  0 - Igrac 2: 0",CENTER_MODE);        
               
            }
   
    //izlaz
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
    BSP_LCD_FillRect(190, 225, 50, 13);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(200,226,(uint8_t*)"EXIT",LEFT_MODE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    
    //NEXT
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
    BSP_LCD_FillRect(2, 225, 50, 13);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(12,226,(uint8_t*)"NEXT",LEFT_MODE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
   
    //grid
    BSP_LCD_SetFont(&Font16);
        int x=2, y=17;
        for(int i=0; i < 9; i++){
            for(int j=0; j < 9; j++){
                BSP_LCD_DrawRect(x,y,26,23);
                if(i>0 && j>0 && mat[i-1][j-1]>='2' && mat[i-1][j-1]<='5' && matr_ind==1) BSP_LCD_DisplayStringAt(x+8,y+7,(uint8_t*)"X",LEFT_MODE);
                if(i>0 && j>0 && mat[i-1][j-1]>='2' && mat[i-1][j-1]<='5' && matr_ind==2) {
                    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
                    BSP_LCD_FillRect(x+1,y+1,25,21);
                    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                }
                if(i>0 && j>0 && mat[i-1][j-1]=='X') {
                    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
                    BSP_LCD_FillRect(x+1,y+1,25,21);
                    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                }else if(i>0 && j>0 && mat[i-1][j-1]=='O') BSP_LCD_FillRect(x+1,y+1,25,21);
                else{
                if(j==0 && i!=0){
                    char red[2];
                    sprintf(red,"%d",i);
                    BSP_LCD_DisplayStringAt(x+8,y+7,(uint8_t*)red,LEFT_MODE);
                }
                if(j!=0 && i==0){
                    char kolona[2];
                    sprintf(kolona,"%c",64+j);
                    BSP_LCD_DisplayStringAt(x+8,y+7,(uint8_t*)kolona,LEFT_MODE);
                }
                }
                x+=26;
            }
            x=2;
            y+=23;
        } 
}
void ocisti(){
    stanje=0;
    jedan_igrac=0;
    dva_igraca=0;
    strategy=1;
    ciji_je_red=0;
    
    for(int i=0; i < 8; i++){
        for(int j=0; j < 8; j++){
            player1[i][j]=0;
            player2[i][j]=0;
            player1_strategy[i][j]=0;
            player2_strategy[i][j]=0;
        }
    }
}
void one_player(){
    int i, j;
    // brodovi: 1-nisu pozic., 0-jesu pozic.
    int ships[4]={1,1,1,1};
    while(ships[0] || ships[1] || ships[2] || ships[3]){
        ships_();
        int m=0, n=0, a=0, b=0, trenutni=0;
        while(1){
            auto koord=touch(1);
            m=koord.first;
            a=koord.second;
            // odabir broda i informacija o njegovoj veličini
            if(a >= 30 && a <=55 && m >= 80 && m <=180 && ships[2]) { trenutni=4; ships[2]=0; break; }
            if(a >= 80 && a <=105 && m >= 80 && m <=155 && ships[1]) { trenutni=3; ships[1]=0; break; }
            if(a >= 130 && a <=155 && m >= 80 && m <=130 && ships[0]) { trenutni=2; ships[0]=0; break; }
            if(a >= 180 && a <=205 && m >= 80 && m <=205 && ships[3]) { trenutni=5; ships[3]=0; break; }
        }
        wait(0.5);
        // crta se matrica igrača i čeka se na pozicioniranje
        start_draw(player_matrix,2);
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_FillRect(2, 225, 50, 13);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_FillRect(190, 225, 50, 13);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_DisplayStringAt(0,5,(uint8_t*)"---PLACE YOUR SHIPS---",CENTER_MODE);
        BSP_LCD_SetFont(&Font20);
        a=0; m=0;
        while(!a || !m ){
            auto koord=touch();
            m=koord.first;
            a=koord.second;
            if(player_matrix[m-1][a-1]!='-') { a=0; m=0; wait(0.5); continue; }
            else break;
        }
        wait(0.5);
        while(!n || !b ){
            auto koord=touch(2);
            n=koord.first;
            b=koord.second;
            // ukoliko unos nije validan: neispravna veličina, dijagonalno pozicioniranje...
            if(player_matrix[n-1][b-1]!='-' || (abs(m-n)!=trenutni-1 && abs(a-b)!=trenutni-1) || (abs(m-n)==trenutni-1 && a!=b) || (abs(a-b)==trenutni-1 && m!=n)) { 
                n=0; b=0; 
                wait(0.5); continue; }
            else break;
        }
        // kako indeksiranje kreće od 0
        m--; n--; a--; b--;
        // ukoliko je pozicioniran horizontalno
        if(m==n){
            int min, max;
            min=(a<b)?a:b;
            max=(a>b)?a:b;
            for(int j=min; j<=max; j++) { 
                player_matrix[m][j] = (max-min+1) + '0';
                BSP_LCD_SetTextColor(LCD_COLOR_RED);
                BSP_LCD_FillRect((j)*26+29, (m)*23+41, 25, 21);
                BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                wait_ms(50);
            } 
        // ukoliko je pozicioniran vertikalno
        } else if(a==b){
            int min, max;
            min=(m<n)?m:n;
            max=(m>n)?m:n;
            for(int j=min; j<=max; j++) {
                player_matrix[j][a] = (max-min+1) + '0';
                BSP_LCD_SetTextColor(LCD_COLOR_RED);
                BSP_LCD_FillRect((a)*26+29, (j)*23+41, 25, 21);
                BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
                if(j!=max)
                wait_ms(50);
            }    
        }
        wait_ms(50);
    }
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0,100,(uint8_t*)"COMPUTER IS",CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,122,(uint8_t*)"PLACING ITS SHIPS",CENTER_MODE);
    wait_ms(50);
    for(int i=2; i <= 5; i++) pozicioniraj(i);
    // igrač koje je potopio    računar koje je potopio
    int brodovi1[4]={2,3,4,5}, brodovi2[4]={2,3,4,5};
    int pog=0, r=0, s=0, stara=-7, ukupno=0, vertikala=0, horizontala=0;
    while(1){
        // sve dok svi brodovi jednog od igrača nisu potopljeni
        if((!brodovi1[0] && !brodovi1[1] && !brodovi1[2] && !brodovi1[3]) || (!brodovi2[0] && !brodovi2[1] && !brodovi2[2] && !brodovi2[3])) break;
        {
            wait_ms(500);
            player();
            wait(1);
            start_draw(comp_matrix);
            BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
            BSP_LCD_FillRect(2, 225, 50, 13);
            BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
            int x=0, y=0;
            // sve dok odabir nije validan: polje već gađano
            while(!x || !y || (comp_matrix[x-1][y-1]=='X' || comp_matrix[x-1][y-1]=='O')){
                auto koord=touch();
                x=koord.first;
                y=koord.second;
            }
            //printf("%d,%d\n",x,y);
            //EXIT
            if(x==9 && y>=7 && y<=8) { 
                stanje=0;
                jedan_igrac=0;
                poeni1=0; poeni2=0;
                for(int i=0; i < 8; i++){
                    for(int j=0; j < 8; j++){
                        comp_matrix[i][j]='-';
                        player_matrix[i][j]='-';
                    }
                }
                return; 
            }
            // ukoliko smo pogodili
            if(comp_matrix[x-1][y-1]>='2' && comp_matrix[x-1][y-1]<='5'){
                brodovi1[comp_matrix[x-1][y-1]-'0'-2]--;
                poeni1++;
                comp_matrix[x-1][y-1]='X';
            }
            else if(comp_matrix[x-1][y-1]=='-'){
                comp_matrix[x-1][y-1]='O';
            }
            wait(1);
            start_draw(comp_matrix);
            BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
            BSP_LCD_FillRect(2, 225, 50, 13);
            BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
            }
        // ukoliko igrač već nije potopio brodove računara
        if((!brodovi1[0] && !brodovi1[1] && !brodovi1[2] && !brodovi1[3]) || (!brodovi2[0] && !brodovi2[1] && !brodovi2[2] && !brodovi2[3])) break;
        {
            // RAČUNAR
            int x, y, n;
            // ispis matrice
            wait_ms(500);
            comp();
            wait(1);
            start_draw(player_matrix, 1);
            BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
            BSP_LCD_FillRect(2, 225, 50, 13);
            BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
            // ukoliko je već pogodio
            if(pog){
                x=r;
                // prvi put: pretpostavljamo da nije vertikalno postavljen brod
                if(!vertikala){
                    for(i=s+1; i < 8; i++){
                        // da ne bi gađao isto mjesto 
                        if(player_matrix[x][i]!='O' && player_matrix[x][i]!='X' && player_matrix[x][i-1]=='X') {
                            y=i; 
                            // ukoliko smo pogodili, znači da je brod horizontalno postavljen
                            if(player_matrix[x][y]>='2' && player_matrix[x][y]<='5') horizontala=1;
                            break;
                        }
                    }
                }
                if(i==8 && !vertikala){
                    for(i=s-1; i >= 0; i--){
                        if(player_matrix[x][i]!='O' && player_matrix[x][i]!='X' && player_matrix[x][i+1]=='X') {
                            y=i; 
                            if(player_matrix[x][y]>='2' && player_matrix[x][y]<='5') horizontala=1;
                            break;
                        }
                    }
                }
                if((i==-1 && !horizontala) || vertikala){
                    y=s;
                    for(i=r-1; i >= 0; i--){
                    if(player_matrix[i][s]!='O' && player_matrix[i][s]!='X' && player_matrix[i+1][s]=='X') {
                        x=i; 
                        if(player_matrix[x][y]>='2' && player_matrix[x][y]<='5') vertikala=1;
                        break;
                    }
                    }
                }
                if(i==-1 && !horizontala){
                    y=s;
                    for(i=r+1; i < 8; i++){
                    if(player_matrix[i][s]!='O' && player_matrix[i][s]!='X' && player_matrix[i-1][s]=='X') {
                        x=i; 
                        if(player_matrix[x][y]>='2' && player_matrix[x][y]<='5') vertikala=1;
                        break;
                    }
                    }
                }
                // ukupno varijabla se koristi kao neki nivo inteligencije za brod od 5 mjesta
                if(player_matrix[x][y]>='2' && player_matrix[x][y]<='5') ukupno++;
                // ukoliko je bod već potopljen (nema odg. pozicije za gađanje) ili smo gađali već 5 puta
                if(i==8 || i==-1 || (r==x && s==y) || ukupno>5) pog=0;
                stara=y;
                x++; y++;
            } 
            // ukoliko nismo pogodili gađaj random mjesto koje već nije gađano
            while(1 && !pog){
                int niz[8]={0}; // niz preostalih mjesta
                n=0; // velicina niza
                x=frandom(0,7);
                for(int i=0; i < 8; i++){
                    if(player_matrix[x][i]!='O' && player_matrix[x][i]!='X' && (abs(x-i))%2==stanje) niz[n++]=i;
                }
                // n - broj nepogođenih mjesta u redu x
                if(n==0) continue;
                else {
                y=frandom_niz(niz,n);
                x++; y++;
                break;
                }
            }
            // ukoliko smo uspjeli pogoditi
            if(player_matrix[x-1][y-1]>='2' && player_matrix[x-1][y-1]<='5'){
                brodovi2[player_matrix[x-1][y-1]-'0'-2]--;
                player_matrix[x-1][y-1]='X';
                poeni2++;
                if(!pog){
                ukupno=0;
                r=x-1;
                s=y-1;
                pog=1;
                horizontala=0; vertikala=0;
                }
            }
            else if (player_matrix[x-1][y-1]=='-'){
                player_matrix[x-1][y-1]='O';
            }
            wait(1);
            start_draw(player_matrix,1);
            BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
            BSP_LCD_FillRect(2, 225, 50, 13);
            BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        }
    }
    // ispis rezultata igre
    win();
    // vraćanje bitnih varijabli na inicijalne vrijednosti
    jedan_igrac=0;
    poeni1=0; poeni2=0;
    for(int i=0; i < 8; i++){
        for(int j=0; j < 8; j++){
            comp_matrix[i][j]='-';
            player_matrix[i][j]='-';
        }
    }
    wait(3);
}
void two_player(){
    BSP_TS_GetState(&TS_State);
    if(TS_State.touchDetected) {
        // Get X and Y position of the first touch post calibrated 
        uint16_t x1 = TS_State.touchX[0];
        uint16_t y1 = TS_State.touchY[0];

        // oboji odabrano polje 
        if( stanje==1 && x1>28 && x1<236 && y1>40 && y1<224){
            isti_potez=1;
            show_played();
              
            BSP_LCD_SetTextColor(LCD_COLOR_RED);
            uint16_t x=((x1-28)/26)*26+3+26, y=((y1-17-23)/23)*23+23+18;
            BSP_LCD_FillRect(x, y, 25, 21); 
              
            x_za_matricu=((x1-28)/26);
            y_za_matricu=((y1-17-23)/23);
        }
        //EXIT (190, 225, 50, 13);
        if( stanje==1 && x1>190 && x1<190+50 && y1>225 && y1<225+13){
            ocisti();
            start();
        }
        //NEXT (2, 225, 50, 13)
        if( stanje==1 && x1>2 && x1<2+50 && y1>225 && y1<225+13){
            if(!ciji_je_red) player1[ y_za_matricu][ x_za_matricu]=1;
            if(ciji_je_red) player2[ y_za_matricu][ x_za_matricu]=1;
            show_played(); 
            wait_ms(20);
            isti_potez=0;
            win();
            if(!kraj_partije){
                ciji_je_red=!ciji_je_red;
                show_played();
            }
            if(kraj_partije){
                ocisti();
                start();
            }
        }
    }
}
void start(){
        ocisti();
        BSP_LCD_Init();
        BSP_LCD_Clear(LCD_COLOR_BLUE);
        BSP_LCD_SetFont(&Font24);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(0,65,(uint8_t*)"START GAME",CENTER_MODE);
        BSP_LCD_SetFont(&Font20);
        BSP_LCD_DisplayStringAt(0,100,(uint8_t*)"1 PLAYER ",CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,130,(uint8_t*)"2 PLAYERS",CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,160,(uint8_t*)"ABOUT GAME",CENTER_MODE);
        while(1){
            BSP_TS_GetState(&TS_State);
            if(TS_State.touchDetected) {
            /* One or dual touch have been detected          */

            /* Get X and Y position of the first touch post calibrated */
            uint16_t x1 = TS_State.touchX[0];
            uint16_t y1 = TS_State.touchY[0];
            //izbor igraca 1
            if(stanje==0 && x1>60 && x1<175 && y1>100 && y1<120){
                stanje=1;
                jedan_igrac=1;
                wait(1);
            }
            //izbor igraca 2
            if(stanje==0 && x1>60 && x1<175 && y1>130 && y1<150){
                stanje=1;
                postavi_strategiju();
                postavi_strategiju();
                
                dva_igraca=1;
                show_played();
                wait(1);
            }
            // about the game
            if(stanje==0 && x1>60 && x1<175 && y1>160 && y1<180){
                uputa();
                wait(1);
                start();
            }
            break;
            }
        }
}
int main() {
    while (1) {
        if(stanje==0) start();
        poeni1=0; poeni2=0;
        if(jedan_igrac) one_player();
        if(dva_igraca) two_player();
        wait_ms(10);
    }
}