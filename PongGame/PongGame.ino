#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Posição dos pinos dos botões
int player1_up = 8;
int player1_down = 9;
int start = 10;
int player2_up = 11;
int player2_down = 12;

// Personalização da barra e da bola
byte playerCharacter[8] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

byte playerCharacter2[8] = {
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001
};

byte ballCharacter[8] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000,
  B00000
};

int delayTime = 200; // Velocidade de movimento das barras e da bola
int score1 = 0;
int score2 = 0;
int waiting = 0;
int isRunning = 0;
int dificuldade = 0;
const char *difficultyOptions[] = {"Facil", "Dificil"};
int selectedDifficulty = 0;

// Armazena as posições das barras
int posPlayer1Lin = 0; 
int posPlayer1Col = 3; 
int posPlayer2Lin = 3; 
int posPlayer2Col = 18; 

// Armazena a posição da bola e suas direções
int posBallLin = 2; 
int posBallCol = 10; 
int ballDirLin = 1;   // 1 == Desce | -1 == Sobe
int ballDirCol = -1;  // 1 == Direita | -1 == Esquerda

void setup()
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, playerCharacter);  // Cria um caractere personalizado da barra na posição 0
  lcd.createChar(1, playerCharacter2); // Cria um caractere personalizado da barra na posição 1
  lcd.createChar(2, ballCharacter);    // Cria um caractere personalizado da bola na posição 2

  pinMode(start, INPUT);
  pinMode(player1_up, INPUT);
  pinMode(player1_down, INPUT);
  pinMode(player2_up, INPUT);
  pinMode(player2_down, INPUT);

  lcd.setCursor(3, 1);        
  lcd.print("Projeto  Final");    
  lcd.setCursor(3, 2);
  lcd.print("Jogo Ping Pong");
  delay(2500);

  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Desenvolvido por");
  lcd.setCursor(2, 2);
  lcd.print("Nicolas e Felipe");
  delay(2000);
  lcd.clear();
}


void loop()
{
  while(isRunning == 0)
  {

    while (dificuldade == 0)
      handleInput(); 

    lcd.clear();
    lcd.setCursor(4,1);
    lcd.print("Para iniciar");
    lcd.setCursor(2,2);
    lcd.print("Clique no Botao");

    if (digitalRead(start) == HIGH)
    {
      lcd.clear();
      lcd.setCursor(5,1);
      lcd.print("Jogadores");
      lcd.setCursor(0,3);
      lcd.print("1 <--");
      lcd.setCursor(15,3);
      lcd.print("--> 2");
      delay(2000);

      for(int i = 3; i > 0; i--)
      {
        lcd.clear();
        lcd.setCursor(2,1);
        lcd.print("O jogo inicia em");
        lcd.setCursor(3,2);
        lcd.print(i);
        lcd.setCursor(5,2);
        lcd.print("segundos...");
        delay(1000);
      }

      isRunning = 1;
    }

    delay(1000);
  }

  //Movimentos
  // Jogador 1
  if (digitalRead(player1_up) == HIGH && posPlayer1Lin > 0) 
    moveBar(0, -1); 
  else if (digitalRead(player1_down) == HIGH && posPlayer1Lin < 3) 
    moveBar(0, 1); 

  // Jogador 2
  if (digitalRead(player2_up) == HIGH && posPlayer2Lin > 0) 
    moveBar(3, -1); 
  else if (digitalRead(player2_down) == HIGH && posPlayer2Lin < 3) 
    moveBar(3, 1);

  //Bola
  moveBall();

  // Mostrando na tela as barras e a bola
  lcd.clear();
  displayBars();
  lcd.setCursor(posBallCol, posBallLin);
  lcd.write((uint8_t)2); // Display da bola

  // Pontuação
  lcd.setCursor(0, 0);
  lcd.print("P1");
  lcd.setCursor(0, 1);  
  lcd.print(score1);
  lcd.setCursor(0, 2);
  lcd.print("P2");
  lcd.setCursor(0, 3);  
  lcd.print(score2);


// Condição de vitória
  if (score1 == 5) 
  {
    score1 = 0;
    score2 = 0;
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("Jogador 1");
    lcd.setCursor(7, 2);
    lcd.print("Ganhou");
    delay(1000);
    lcd.clear();
    // Reseta para o padrão
    posPlayer1Lin = 0; 
    posPlayer1Col = 3; 
    posPlayer2Lin = 3; 
    posPlayer2Col = 18;
    isRunning = 0;
    selectedDifficulty = 0;  
    dificuldade = 0;
  } 
  else if(score2 == 5)
  {
    score1 = 0;
    score2 = 0;
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("Jogador 2");
    lcd.setCursor(7, 2);
    lcd.print("Ganhou");
    delay(1000);
    lcd.clear();
    posPlayer1Lin = 0; 
    posPlayer1Col = 3; 
    posPlayer2Lin = 3; 
    posPlayer2Col = 18;
    isRunning = 0;
    selectedDifficulty = 0;  
    dificuldade = 0;
  }

  // Velocidade do jogo
  Serial.println(delayTime); 
  delay(delayTime);
}


void displayBars() 
{
  // Display da barra do jogador 1
  lcd.setCursor(posPlayer1Col, posPlayer1Lin);
  lcd.write((uint8_t)0); 

  // Display da barra do jogador 2
  lcd.setCursor(posPlayer2Col, posPlayer2Lin);
  lcd.write((uint8_t)1); 
}

void moveBar(int linha, int dir) 
{
  // Move a barra de acordo com o input do botão
  if (linha == 0) 
  {
    // Move a barra do jogador1
    lcd.setCursor(posPlayer1Col, posPlayer1Lin);
    lcd.print(" "); // Limpa a posição atual
    posPlayer1Lin += dir;
  } else if (linha == 3) 
  {
    // Move a barra do jogador2
    lcd.setCursor(posPlayer2Col, posPlayer2Lin);
    lcd.print(" "); 
    posPlayer2Lin += dir;
  }

  // Atualiza o display
  displayBars();
}

void moveBall()
{
  // Move a bola
  lcd.setCursor(posBallCol, posBallLin);
  lcd.print(" "); 

  // Atualiza a posição da bola
  posBallLin += ballDirLin;
  posBallCol += ballDirCol;

  // Verificar as colisões 
  // Colisão com o chão e o teto
  if (posBallLin == 0 || posBallLin == 3)   
    ballDirLin *= -1; // Inverte a direção 

  // Colisão com a barra do jogador1
  if (posBallCol == posPlayer1Col && posBallLin == posPlayer1Lin) 
  {
    if(selectedDifficulty == 1)
      if(delayTime > 50)
        delayTime -= 10;
    ballDirCol *= -1; // Inverte a direção
    if (posBallLin == 3)
      posBallCol += 1;
  }

  // Colisão com a barra do jogador2
  if (posBallCol == posPlayer2Col && posBallLin == posPlayer2Lin) 
  {
    if(selectedDifficulty == 1)
      if(delayTime > 50)
        delayTime -= 10;
    ballDirCol *= -1; // Inverte a direção
    if (posBallLin == 0])
      posBallCol -= 1;
    
  }


  // Verifica a pontuação
  if (posBallCol == 0) 
  {
    score2++;
    resetBall();
  } else if (posBallCol == 19) 
  {
    score1++;
    resetBall();
  }
}

void resetBall() 
{
  // Reseta a posição da bola 
  posBallLin = rand() % 2 + 1;
  posBallCol = 10;
}


void handleInput()
{
  lcd.setCursor(0, 1);
  lcd.print("Dificuldade:");
  lcd.setCursor(0, 2);
  lcd.print(">");
  lcd.setCursor(2, 2);
  lcd.print(difficultyOptions[selectedDifficulty]);

  // Dificuldade: Díficil
  if (digitalRead(player1_up) == HIGH)
  {
    selectedDifficulty = 1;
    lcd.setCursor(0, 2);
    lcd.print("      ");
    delay(300);
  }

  // Dificuldade: Fácil
  if (digitalRead(player1_down) == HIGH)
  {
    selectedDifficulty = 0;
    lcd.setCursor(0, 2);
    lcd.print("         ");
    delay(300);
  }

  if (digitalRead(start) == HIGH)
  {
    lcd.clear();
    lcd.setCursor(4, 1);
    lcd.print("Boa Sorte!!");
    lcd.setCursor(0, 2);
    lcd.print("Dificuldade: ");
    lcd.print(difficultyOptions[selectedDifficulty]);
    delay(2000);
    dificuldade = 1;
  }
}
