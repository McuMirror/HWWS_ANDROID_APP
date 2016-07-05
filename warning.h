#ifndef WARNING_H
#define WARNING_H

#include<QString>
#include<QSettings>

class Warning
{
public:
	Warning();
	void setName(QString Name);
	void setHeight(int height);
	void setTriggerState(bool triggered);
	void setStateWarningAtOverUnderheight(bool StateWarningAtOverUnderheight);

	QString getName(void);
	int getHeight(void);
	bool getTriggerState(void);
	bool getStateWarningAtOverUnderheight(void);



private:
	QString Name;
	int height;
	bool triggered = false;
	bool StateWarningAtOverUnderheight;

};

#endif // WARNING_H
