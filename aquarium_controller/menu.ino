////////////////////////////////////////////////////////////////////////////////              
// Arduino Based Freshwater Aquarium Control
////////////////////////////////////////////////////////////////////////////////                        
// Tab: Menu
////////////////////////////////////////////////////////////////////////////////

void printMenuPosition(byte pos){
  switch (pos) {
    case 1:
      if(maintenanceMode)
        lcd.print(F("Exit Maint."));
      else
        lcd.print(F("Maintenace"));
      break;
    case 2:
      lcd.print(F("Fert. run"));
      break;
    case 3:
      lcd.print(F("Exit Menu"));
      break;
  }  
}

void renderMenu(){
  lcd.clear();
  lcd.print(F("> "));
  printMenuPosition(menuPosition);
  if(menuPosition < menuMaxpos){
    lcd.setCursor(2, 1);
    printMenuPosition(menuPosition+1);
  }
}

void menuSelect(byte selection){
  switch (selection) {
    case 1:
      if(maintenanceMode)
        maintenanceMode = false;
      else
        maintenanceMode = true;
      break;
    case 2:
      Fertilize();
      break;
    case 3:
      break;
  }
}

void checkMenu(){
  uint8_t buttons = lcd.readButtons();
  
  if (buttons) {
    if (buttons & BUTTON_DOWN) {
      if(inMenu){
        if(menuPosition < menuMaxpos){
          menuPosition++;
          renderMenu();
        }
      }else{
        inMenu = true;
        menuPosition = 1;
        renderMenu();
      }
    }
    if (buttons & BUTTON_UP) {
      if(inMenu && (menuPosition > 1)){
        menuPosition--;
        renderMenu();
      }
    }
    if (buttons & BUTTON_SELECT) {
      if(inMenu){
        menuSelect(menuPosition);
        inMenu = false;
        lcd.clear();
        checkSwitches();
        update();
      }
    }
  }
}
