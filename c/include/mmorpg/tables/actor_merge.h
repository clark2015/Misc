#ifndef actor_merge_h
#define actor_merge_h

#include "actor_fixed.h"
#include "actor_detail.h"
#include "character_prop.h"

//下面是手工定义的
#pragma pack(push,1)

struct actor_merge
{
	unsigned short 	fix_len;
	unsigned short 	detail_len;
	unsigned short 	prop_len;
	unsigned short 	reserved;
	unsigned short 	reserved1;
	
	actor_fixed			fixed;
	actor_detail		detail;
	character_prop_nt	prop;
	
			actor_fixed*		get_actor_fixed()			{	return &fixed;	}
	const 	actor_fixed*		get_actor_fixed()	const	{	return &fixed;	}
			actor_detail*		get_actor_detail()			{	return (actor_detail*)  (((char*)this) + sizeof(short)*5 + fix_len);	}
	const	actor_detail*		get_actor_detail()	const	{	return (actor_detail*)  (((char*)this) + sizeof(short)*5 + fix_len);	}			
			character_prop_nt*	get_character_prop()		{	return (character_prop_nt*)(((char*)this) + sizeof(short)*5 + fix_len + detail_len);	}
	const	character_prop_nt*	get_character_prop()const	{	return (character_prop_nt*)(((char*)this) + sizeof(short)*5 + fix_len + detail_len);	}			

	actor_merge()
	{
		fix_len		= sizeof(actor_fixed);
		detail_len	= sizeof(actor_detail);
		prop_len	= sizeof(character_prop_nt);
		reserved	= 0;
		reserved1	= 0;
	}
};
#pragma pack(pop)


#endif
