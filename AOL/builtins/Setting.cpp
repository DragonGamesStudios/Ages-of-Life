#include "Setting.h"

SettingValue::~SettingValue()
{
	d_value = 0;
	i_value = 0;
	s_value = "";
	b_value = false;
}
