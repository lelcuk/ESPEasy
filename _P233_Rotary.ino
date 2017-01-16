//#######################################################################################################
//#################################### Plugin 233: Rotary Encoder #######################################
//#######################################################################################################

#define PLUGIN_233
#define PLUGIN_ID_233         233
#define PLUGIN_NAME_233       "Rotary Encoder"
#define PLUGIN_VALUENAME1_233 "Value"
#define PLUGIN_VALUENAME2_233 "Total"
#define PLUGIN_VALUENAME3_233 "Time"

#define PLUGIN_233_MINVAL  0
#define PLUGIN_233_MAXVAL 90


void Plugin_233_A_interrupt1() ICACHE_RAM_ATTR;
void Plugin_233_A_interrupt2() ICACHE_RAM_ATTR;
void Plugin_233_A_interrupt3() ICACHE_RAM_ATTR;
void Plugin_233_A_interrupt4() ICACHE_RAM_ATTR;
void Plugin_233_B_interrupt1() ICACHE_RAM_ATTR;
void Plugin_233_B_interrupt2() ICACHE_RAM_ATTR;
void Plugin_233_B_interrupt3() ICACHE_RAM_ATTR;
void Plugin_233_B_interrupt4() ICACHE_RAM_ATTR;

unsigned long Plugin_233_pulseCounter[TASKS_MAX];
unsigned long Plugin_233_pulseTotalCounter[TASKS_MAX];
unsigned long Plugin_233_pulseTime[TASKS_MAX];
unsigned long Plugin_233_pulseTimePrevious[TASKS_MAX];
int           Plugin_233_ValuePrevious[TASKS_MAX];

boolean Plugin_233(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_233;
        Device[deviceCount].Type = DEVICE_TYPE_DUAL;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 3;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_233);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_233));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_233));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_233));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        char tmpString[128];
        sprintf_P(tmpString, PSTR("<TR><TD>Debounce Time (mSec):<TD><input type='text' name='plugin_233' value='%u'>"), Settings.TaskDevicePluginConfig[event->TaskIndex][0]);
        string += tmpString;

        byte choice = Settings.TaskDevicePluginConfig[event->TaskIndex][1];
        String options[3];
        options[0] = F("Delta");
        options[1] = F("Delta/Total/Time");
        options[2] = F("Total");
        int optionValues[3];
        optionValues[0] = 0;
        optionValues[1] = 1;
        optionValues[2] = 2;
        string += F("<TR><TD>Counter Type:<TD><select name='plugin_233_countertype'>");
        for (byte x = 0; x < 3; x++)
        {
          string += F("<option value='");
          string += optionValues[x];
          string += "'";
          if (choice == optionValues[x])
            string += F(" selected");
          string += ">";
          string += options[x];
          string += F("</option>");
        }
        string += F("</select>");

        if (choice !=0)
          string += F("<span style=\"color:red\">Total count is not persistent!</span>");

        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        String plugin1 = WebServer.arg("plugin_233");
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = plugin1.toInt();
        String plugin2 = WebServer.arg("plugin_233_countertype");
        Settings.TaskDevicePluginConfig[event->TaskIndex][1] = plugin2.toInt();
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SHOW_VALUES:
      {
        string += F("<div class=\"div_l\">");
        string += ExtraTaskSettings.TaskDeviceValueNames[0];
        string += F(":</div><div class=\"div_r\">");
        string += Plugin_233_pulseCounter[event->TaskIndex];
        string += F("</div><div class=\"div_br\"></div><div class=\"div_l\">");
        string += ExtraTaskSettings.TaskDeviceValueNames[1];
        string += F(":</div><div class=\"div_r\">");
        string += Plugin_233_pulseTotalCounter[event->TaskIndex];
        string += F("</div><div class=\"div_br\"></div><div class=\"div_l\">");
        string += ExtraTaskSettings.TaskDeviceValueNames[2];
        string += F(":</div><div class=\"div_r\">");
        string += Plugin_233_pulseTime[event->TaskIndex];
        string += F("</div>");
        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        String log = F("INIT : Rotary ");
        log += Settings.TaskDevicePin1[event->TaskIndex];
        addLog(LOG_LEVEL_INFO,log);
        pinMode(Settings.TaskDevicePin1[event->TaskIndex], INPUT_PULLUP);
        pinMode(Settings.TaskDevicePin2[event->TaskIndex], INPUT_PULLUP);
        Plugin_233_rotaryinit(Settings.TaskDevicePin1[event->TaskIndex],Settings.TaskDevicePin2[event->TaskIndex], event->TaskIndex);
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        UserVar[event->BaseVarIndex] = Plugin_233_pulseCounter[event->TaskIndex];
        UserVar[event->BaseVarIndex+1] = Plugin_233_pulseTotalCounter[event->TaskIndex];
        UserVar[event->BaseVarIndex+2] = Plugin_233_pulseTime[event->TaskIndex];

        switch (Settings.TaskDevicePluginConfig[event->TaskIndex][1])
        {
          case 0:
          {
            event->sensorType = SENSOR_TYPE_SINGLE;
            UserVar[event->BaseVarIndex] = Plugin_233_pulseCounter[event->TaskIndex];
            break;
          }
          case 1:
          {
            event->sensorType = SENSOR_TYPE_TRIPLE;
            UserVar[event->BaseVarIndex] = Plugin_233_pulseCounter[event->TaskIndex];
            UserVar[event->BaseVarIndex+1] = Plugin_233_pulseTotalCounter[event->TaskIndex];
            UserVar[event->BaseVarIndex+2] = Plugin_233_pulseTime[event->TaskIndex];
            break;
          }
          case 2:
          {
            event->sensorType = SENSOR_TYPE_SINGLE;
            UserVar[event->BaseVarIndex] = Plugin_233_pulseTotalCounter[event->TaskIndex];
            break;
          }
        }
        success = true;
        break;
      }
  }
  return success;
}


/*********************************************************************************************\
 * Check Rotary incoder changes (called from irq handler)
\*********************************************************************************************/
void Plugin_233_rotarycheck(boolean pin,byte Index)
{
  unsigned long PulseTime=millis() - Plugin_233_pulseTimePrevious[Index];
  if (PulseTime > Settings.TaskDevicePluginConfig[Index][0]) // check with debounce time for this task
    {
      cli(); 
      
      int MSB = digitalRead(Settings.TaskDevicePin1[Index]); //MSB = most significant bit
      int LSB = digitalRead(Settings.TaskDevicePin2[Index]); //LSB = least significant bit
      int encoded = (MSB << 1) |LSB;      //converting the 2 pin value to single number
      int sum  = (Plugin_233_ValuePrevious[Index] << 2) | encoded; //adding it to the previous encoded value
      
      if ((sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) && (Plugin_233_pulseCounter[Index] < PLUGIN_233_MAXVAL))Plugin_233_pulseCounter[Index]++;
      if ((sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) && (Plugin_233_pulseCounter[Index] > PLUGIN_233_MINVAL))Plugin_233_pulseCounter[Index]--;

      Plugin_233_ValuePrevious[Index] = encoded; //store this value for next time
      
      Plugin_233_pulseTime[Index] = PulseTime;
      Plugin_233_pulseTimePrevious[Index]=millis();

      sei();
    }
}


/*********************************************************************************************\
 * Rotary Counter IRQ handlers
\*********************************************************************************************/
void Plugin_233_A_interrupt1()
{
  Plugin_233_rotarycheck(0,0);
}
void Plugin_233_A_interrupt2()
{
  Plugin_233_rotarycheck(0,1);
}
void Plugin_233_A_interrupt3()
{
  Plugin_233_rotarycheck(0,2);
}
void Plugin_233_A_interrupt4()
{
  Plugin_233_rotarycheck(0,3);
}

void Plugin_233_B_interrupt1()
{
  Plugin_233_rotarycheck(1,0);
}
void Plugin_233_B_interrupt2()
{
  Plugin_233_rotarycheck(1,1);
}
void Plugin_233_B_interrupt3()
{
  Plugin_233_rotarycheck(1,2);
}
void Plugin_233_B_interrupt4()
{
  Plugin_233_rotarycheck(1,3);
}

/*********************************************************************************************\
 * Init Rotary Counters
\*********************************************************************************************/
void Plugin_233_rotaryinit(byte Par1,byte Par2, byte Index)
{
  // Init IO pins
  String log = F("ROTRY: Init");
  addLog(LOG_LEVEL_INFO,log);

  switch (Index)
  {
    case 0:
      attachInterrupt(Par1, Plugin_233_A_interrupt1, CHANGE);
      attachInterrupt(Par2, Plugin_233_B_interrupt1, CHANGE);
      break;
    case 1:
      attachInterrupt(Par1, Plugin_233_A_interrupt2, CHANGE);
      attachInterrupt(Par2, Plugin_233_B_interrupt2, CHANGE);
      break;
    case 2:
      attachInterrupt(Par1, Plugin_233_A_interrupt3, CHANGE);
      attachInterrupt(Par2, Plugin_233_B_interrupt3, CHANGE);
      break;
    case 3:
      attachInterrupt(Par1, Plugin_233_A_interrupt4, CHANGE);
      attachInterrupt(Par2, Plugin_233_B_interrupt4, CHANGE);
      break;
  }
}
