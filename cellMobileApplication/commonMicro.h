#pragma once

#define safe_delete(p)		{if(p != nullptr){delete p; p = nullptr;};}
#define empty_break(obj)	{if(!(obj)) break;}