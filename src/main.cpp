#include <Arduino.h>
#include <cstdlib>
#include <ctime>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

#define VRX_PIN  12 // pin connected to VRX pin
#define VRY_PIN  14 // pin connected to VRY pin
#define SW_PIN   27 // pin connected to SW pin

//Define treshhold for controller
#define RIGHT_TRESH 3500
#define LEFT_TRESH 500
#define UP_TRESH 500
#define DOWN_TRESH 3500

ezButton button(SW_PIN);

int x_Value = 0;
int y_Value = 0;
int b_Value = 0;

int x_Cursor = 0;
// Array for saving y_Cursor values
int y_Cursor[5] = {0,0,0,0,0};

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);


//Controller init

 //Class containing all functions for the Game
 class Game{
 private:
    int score = 0;
    int checks = 6;
    String currentWord;
    String charsToCheck;

 public:
    //User Guess
    String currentGuess="*****";
    //Alphabet
    char abc[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

    //CSetNewWord
    void setNewWord(){
            //String of words for the game
        String words[28] = {"AFFE","ABEND","BADEN","BIRNE","COACH","DONAU","ENZYM","FLUOR","GHANA","HUMOR","IMKER","JEANS","KOALA","LACHS","MAINZ","NATUR","OTTER","PUDEL","QUALM","RAUCH","SALSA","TACHO","UNMUT","VISUM","WAAGE","XENON","YACHT","ZECKE"};


        //Generating random number
        srand(time(0));
        int random= rand()% 28;

        //Returning Random word
        currentWord = words[random];
    }

    //Check the user guesses
    //Function Returns 0 if user guessed correctly
    String checkGuess(){
      String answer = "*****";
      charsToCheck = "";

      if (currentGuess.equals(currentWord))
      {
        return "0";
      }
      
        for (int i = 0; i < currentWord.length(); i++)
        {
          if (currentGuess[i]==currentWord[i])
          {
            answer[i] = currentWord[i];
          }
          else{
            answer[i] = '*';
            charsToCheck += currentGuess[i];
          }
        }
      return answer;
    }

    //Check if letters in guess are in the word but at the wrong position
    String inWord(){
      String output;

      for (int i = 0; i < charsToCheck.length(); i++)
      {
        if (currentWord.indexOf(charsToCheck[i])!= -1)
        {
          output += charsToCheck[i];
        }
        
      }
      return output;
    }

    //Getter and Setters
    int getScore(){return score;}
    void setScore(int newScore){score = newScore;}

    String getCurrentWord(){return currentWord;}
    void setCurrentGuess(String guess){guess = currentGuess;}

    int getChecks(){return checks;}
    void setChecks(int newChecks){checks = newChecks;}

 };

//Construct Object Game
Game g;

//LCD
//Init for LCD
void lcdSetup(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("*****#");
  lcd.setCursor(8, 0);
  lcd.print("Checks:");
  lcd.setCursor(15,0);
  lcd.print(g.getChecks());
  lcd.setCursor(0,1);
  lcd.print("In Word:");
  lcd.home();
  lcd.cursor();
}

//func for accessing alphabet
void abcMenu(){
  
  lcd.write(g.abc[y_Cursor[x_Cursor]]);
  lcd.setCursor(x_Cursor,0);
  g.currentGuess[x_Cursor] = g.abc[y_Cursor[x_Cursor]];
  delay(150);
}
//Winner function
void winner(){
  while (true)
  {
  lcd.clear(); 
  lcd.cursor_off();
  lcd.setCursor(1,0);
  delay(1000);
  lcd.print("WINNER WINNER!");
  delay(1000);
  lcd.setCursor(1,1);
  lcd.print("Chicken Dinner");
  delay(1000);
  }
}

//Command Loop
void controllInput(){
  x_Value = analogRead(VRX_PIN);
  y_Value = analogRead(VRY_PIN);
  button.loop();
    if(x_Value > RIGHT_TRESH){
      if (x_Cursor == 5)
      {
        return;
      }
      else
      {
        x_Cursor += 1;
      }
      lcd.setCursor(x_Cursor,0);
      lcd.cursor();
      delay(150);
    }
    else if (x_Value < LEFT_TRESH)
    {
      if (x_Cursor == 0)
      {
        return;
      }
      else
      {
        x_Cursor -= 1;
      }
      lcd.setCursor(x_Cursor,0);
      lcd.cursor();
      delay(150);
    }
    else if (y_Value < UP_TRESH){
      y_Cursor[x_Cursor] -= 1;
      abcMenu();
      delay(150);
    }
    else if (y_Value > DOWN_TRESH)
    {
      abcMenu();
      y_Cursor[x_Cursor] += 1;
      delay(150);
    }
    else if (button.isPressed()&& x_Cursor ==5)
    { 
      for (int i = 0; i < 5; i++)
      {
        y_Cursor[i]=0;
      }
      x_Cursor = 0;
      
      lcd.home();
      lcd.cursor_off();
      lcd.cursor();
      if (g.checkGuess().equals("0"))
      {
        winner();
      }
      else
      {
      g.setChecks(g.getChecks()-1);
      lcdSetup();
      lcd.print(g.checkGuess());
      lcd.setCursor(8,1);
      lcd.print(g.inWord());
      lcd.home();
      }
    }
    
       
  }
  

void setup() {
//Setup Display
  lcd.init();
  lcd.backlight();
  delay(250);
  lcd.noBacklight();
  delay(1000);
  lcd.backlight();
  delay(1000);
  button.setDebounceTime(50);
  lcdSetup();
  g.setNewWord();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly
  controllInput();
  
  
}