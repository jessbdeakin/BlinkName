
#define LED_PIN D7

#define DOT_DURATION_MS 180
#define DASH_DURATION_MS (3*DOT_DURATION_MS)
#define LETTER_SEPARATOR_DURATION_MS (3*DOT_DURATION_MS)

uint8_t morseEncodingLengths[26] = {
    2, 4, 4, 3, 1, 4, 3,
    4, 2, 4, 3, 4, 2, 2,
    3, 4, 4, 3, 3, 1, 3,
    4, 3, 4, 4, 4
};

uint8_t morseEncodings[26] = {
    0b01, 0b1000, 0b1010, 0b100, 0b0, 0b0010, 0b110,
    0b0000, 0b00, 0b0111, 0b101, 0b0100, 0b11, 0b10,
    0b111, 0b0110, 0b1101, 0b010, 0b000, 0b1, 0b001,
    0b0001, 0b011, 0b1001, 0b1011, 0b1100
};

unsigned int morseSymbolDuration[2] = {DOT_DURATION_MS, DASH_DURATION_MS};

enum BlinkState {
    IDLE,
    START_MESSAGE,
    START_SYMBOL,
    SYMBOL,
    START_SYMBOL_SEPARATOR,
    SYMBOL_SEPARATOR,
    START_LETTER_SEPARATOR,
    LETTER_SEPARATOR
};

BlinkState blinkState = BlinkState::IDLE;
const char * blinkName = "jess";
unsigned int blinkLetterIndex = 0;
unsigned int blinkSymbolIndex = 0;
unsigned int blinkStartTime = 0;

void buttonClickHandler(system_event_t event){
    blinkState = BlinkState::START_MESSAGE;
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    
    System.on(button_click, buttonClickHandler);
    Serial.begin(9600);
    Serial.println("Setup complete.");
}

void loop() {
    switch(blinkState){
        case BlinkState::IDLE: break;
        
        case BlinkState::START_MESSAGE:
        
            Serial.println("Starting message...");
        
            blinkLetterIndex = 0;
            blinkSymbolIndex = 0;
            
            blinkState = BlinkState::START_SYMBOL;
            
        break;
        
        case BlinkState::START_SYMBOL: {
            
            Serial.println("Starting symbol...");
        
            if(blinkLetterIndex >= strlen(blinkName)){
                blinkState = BlinkState::START_MESSAGE;
                digitalWrite(LED_PIN, LOW);
                return;
            }
        
            char letter = blinkName[blinkLetterIndex];
            
            if(blinkSymbolIndex >= morseEncodingLengths[letter - 'a']){
                blinkSymbolIndex = 0;
                blinkLetterIndex++;
                blinkState = BlinkState::START_LETTER_SEPARATOR;
                return;
            }
            
            digitalWrite(LED_PIN, HIGH);
            blinkStartTime = millis();
            
            blinkState = BlinkState::SYMBOL;
            
        } break;
            
        case BlinkState::SYMBOL: {
            
            char letter = blinkName[blinkLetterIndex];
            uint8_t encoding = morseEncodings[letter - 'a'];
            
            uint8_t symbol = encoding;
            symbol >>= morseEncodingLengths[letter - 'a'] - 1 - blinkSymbolIndex;
            symbol &= 0b1;
            
            if( millis() - blinkStartTime >= morseSymbolDuration[symbol] ){
                blinkSymbolIndex++;
                blinkState = BlinkState::START_SYMBOL_SEPARATOR;
            }
            
        } break;
        
        case BlinkState::START_SYMBOL_SEPARATOR:
        
            Serial.println("Starting symbol separator...");
        
            digitalWrite(LED_PIN, LOW);
            blinkStartTime = millis();
            
            blinkState = BlinkState::SYMBOL_SEPARATOR;
        
        break;
        
        case BlinkState::SYMBOL_SEPARATOR:
            
            if( millis() - blinkStartTime >= DOT_DURATION_MS ){
                blinkState = BlinkState::START_SYMBOL;
            }
            
        break;
        
        case BlinkState::START_LETTER_SEPARATOR:
        
            Serial.println("Starting letter separator...");
        
            digitalWrite(LED_PIN, LOW);
            blinkStartTime = millis();
            
            blinkState = BlinkState::LETTER_SEPARATOR;
        
        break;
        
        case BlinkState::LETTER_SEPARATOR:
        
            if( millis() - blinkStartTime >= LETTER_SEPARATOR_DURATION_MS ){
                blinkState = BlinkState::START_SYMBOL;
            }
        
        break;
    }
}


