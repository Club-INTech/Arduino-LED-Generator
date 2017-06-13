const int
pinB1 = 0,
pinB2 = 1, //Broches analogiques d'IHM
pinRandom = 5, //Broche analogique connectée à l'oxygène
pinOn = 11, 
pinD1 = 4, //Broches de l'indicateur à DEL (super important)
pinD2 = 5,
pinD3 = 6;

unsigned int code[3];
/*
Code sous forme a;b;c pour :
a fois '1', b fois '2' et c fois '1'
avec a,b,c appartenant à [2..8]
*/

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(pinRandom));
  pinMode(pinD1, OUTPUT);
  pinMode(pinD2, OUTPUT);
  pinMode(pinD3, OUTPUT);
  pinMode(pinOn, OUTPUT);
  wakeUpOracle(code);
  interrogateOracle(code);  
}

void loop()
{
  if(checkRollable()) // TODO : vérifier le cas dual
    doABarrelRoll();
  else
    Serial.println("That's not how you do a barrel roll.");
  
}

void doABarrelRoll()
{
  Serial.println("Do a barrel roll !");
  digitalWrite(pinOn, HIGH);
  delay(1000); // 1s est largement suffisant, après on a le tournis
  digitalWrite(pinOn, LOW);
}

void wakeUpOracle(unsigned int code[])
{
    // TODO L'Oracle étant imprévisible, on utilise la version de secours plus fiable
  code[1]=3;
  code[2]=code[1];
  code[0]=code[1]+code[1+1]-1;

  /*
  for(int i=0; i<3; i++)
  {
    code[i] = random(2, 8);
  }
  */
}

void interrogateOracle(unsigned int code[])
{
  for(int i=0; i<3; i++)
  {
    Serial.println(code[i]);
    if(code[i]%2 == 1)
      digitalWrite(pinD1, HIGH);

    if((code[i]/2)%2 == 1)
      digitalWrite(pinD2, HIGH);

    if((code[i]/4)%2 == 1)
      digitalWrite(pinD3, HIGH);
    
    delay(2000);
    digitalWrite(pinD1, LOW);
    digitalWrite(pinD2, LOW);
    digitalWrite(pinD3, LOW);
    delay(500);
  }
    delay(2000);
}

boolean checkRollable()
{
  // self-explanatory
  int read_value = -1;
  return compte_entree(1, &read_value)==code[0] && read_value == 2
  && compte_entree(2, &read_value)==code[1]-1 && read_value == 1
  && compte_entree(1, &read_value)==code[2]-1 && read_value == 0;
}

int compte_entree(int expected_value, int* read_value)
{
  int compteur = 0;
  *read_value = lecture_attente();
  while(*read_value == expected_value)
  {
    compteur++;
    *read_value = lecture_attente();
  }
    Serial.print("Compteur: ");
    Serial.println(compteur);
    Serial.print("read_value: ");
    Serial.println(*read_value);

  return compteur;
}

int lecture_attente()
{
 const int seuil_haut = 300; // seuil_haut - seuil_bas = 2 * seuil_bas
 const int seuil_bas = 100;
 const int delay_value = 20;
 int nb = 2000/delay_value;
 int out = lecture(seuil_haut);
   digitalWrite(pinD1, LOW);
   digitalWrite(pinD2, LOW);
   digitalWrite(pinD3, LOW);
 while(out == 0 && nb > 0)
 {
   nb--;
   delay(delay_value);
   out = lecture(seuil_haut);
 }
 delay(200); // pour la led
 if(out == 0) // rien n'est détecté
 {
   Serial.println("0"); 
   digitalWrite(pinD1, HIGH);
   return out;
 }

 nb = 5000/delay_value;
 
 while(lecture(seuil_bas) != 0 && nb > 0)
 {
   nb--;
   delay(delay_value);  // sinon on attend de revenir à 0 avant de retourner la valeur
 }

 // TODO : ça peut arriver ? 
 if(nb <= 0)
 {
   Serial.println("Non, non et non !"); 
   digitalWrite(pinD1, HIGH);
   return 0; // > 5 secondes: annulation
 }
 if(out == 1)
    digitalWrite(pinD2, HIGH);
 else if(out == 2)
    digitalWrite(pinD3, HIGH);

 Serial.println(out); 
 return out;   
}

int lecture(int seuil)
{
  int diff_minimal = 200; // coeff de Spearman
  int a1, a2;
  a1 = analogRead(pinB1);
  a2 = analogRead(pinB2);
  
  if(a1 > seuil && a1-a2 >= diff_minimal)
    return 1;
  else if(a2 > seuil && a2-a1 >= diff_minimal)
    return 2;
  else
    return 0;
}

