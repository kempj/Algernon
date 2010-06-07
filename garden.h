#ifndef GARDEN_FOR_ALGERNON
#define GARDEN_FOR_ALGERNON 0

#include <iostream>
#include <fstream>
#include <string>
#include "graphalterations.h"

#define garden_debug true

class Garden
{
private:
	std::fstream seed_source;
	std::ofstream seed_storage;

	int seeds_processed;
	AlterationLib alter_lib;


	std::string current_seed;
public:
	Garden();
	bool OpenSeedPack(std::string path);
	bool OpenSeedStorage(std::string path);
	
	bool LoadNextSeed();
	bool PlantSeed(std::string folder);
	//bool PlantSprout(std::string folder);
	
	int GetSeedsProcessed();
};
Garden::Garden()
{
	current_seed = "";
	seeds_processed = 0;
}
bool Garden::OpenSeedPack(std::string path)
{
	seed_source.open(path.c_str());
	if(!seed_source)
	{
		if(garden_debug)
		{
			std::cout << "-ERROR-\nFILE: garden.h\nFUNCTION: bool Garden::OpenSeedPack(std::string path)\nMESSAGE: file not found or corrupt\n";
		}
		return false;
	}
	seed_source >> current_seed;
	if(!seed_source)
	{
		if(garden_debug)
		{
			std::cout << "-ERROR-\nFILE: garden.h\nFUNCTION: bool Garden::OpenSeedPack(std::string path)\nMESSAGE: file empty or corrupt\n"; 
		}
		return false;
	}

	return true;
}

bool Garden::OpenSeedStorage(std::string path)
{
	seed_storage.open(path.c_str());
	if(!seed_source)
	{
		if(garden_debug)
		{
			std::cout << "-ERROR-\nFILE: garden.h\nFUNCTION: bool Garden::OpenSeedStorage(std::string path)\nMESSAGE: file could not be created\n"; 
		}
		return false;
	}

	return true;
}

bool Garden::LoadNextSeed()
{
	seed_source >> current_seed;
	if(!seed_source)
	{
		if(garden_debug)
		{
			std::cout << "-ERROR-\nFILE: garden.h\nFUNCTION: bool Garden::LoadNextSeed()\nMESSAGE: no more seeds\n"; 
		}
		return false;
	}
	return true;
}
bool Garden::PlantSeed(std::string folder)
{
	bool do_alter = true;
	long long unsigned int args[4];
	int alteration_count = alter_lib.GetListSize(); //get size of alteration library
	int alteration_index = 0; //current alteration to use
	std::fstream file;
	std::string seed_profile = folder;
	std::string file_name;
	std::string new_seed = current_seed; //change default value to "" ?
	std::string tmpStr;

	seed_profile += current_seed + ".txt"; //a profile that indicates if seed was processed
	file.open(seed_profile.c_str(),std::ios::in);

	if(file)
	{
		if(garden_debug)
		{
			std::cout << "-ERROR-\nFILE: garden.h\nFUNCTION: bool Garden::PlantSeed(str::string folder)\nMESSAGE: sprout exists\n"; 
		}
		return false;
	}

	file.close();
	file.clear();

	folder += current_seed + "\\"; //folder for sprouts
	tmpStr = "mkdir \"" + folder + "\"";
	system(tmpStr.c_str());

	while(alteration_index < alteration_count)
	{
		file_name = folder + alter_lib.GetAlterationName(alteration_index) + ".txt";
		file.open(file_name.c_str(),std::ios::out);
		args[0] = 1;
		args[1] = 1;
		args[2] = 1;
		args[3] = 1;
		while(do_alter)
		{
			if(garden_debug)
			{
				std::cout << alter_lib.GetAlterationName(alteration_index) 
					<<" with " << args[0] << " " << args[1] << " " << args[2] << " " << args[3] << '\n'
					<<'\t'<<"seed: " << current_seed << '\n';
			}
			new_seed = alter_lib.DoAlteration(alteration_index,current_seed,args[0],args[1],args[2],args[3]);
			if(garden_debug)
			{
				std::cout << "result: " << new_seed << '\n';
			}
			if(new_seed == current_seed) //alteration ineffective
			{
				if(args[0]!= 1)
				{
					args[0]=1;
					args[1]++;
				}
				else if(args[1]!=1)
				{
					args[1]=1;
					args[2]++;
				}
				else if(args[2]!=1)
				{
					args[2]=1;
					args[3]++;
				}
				else
				{
					do_alter = false;
				}
			}
			else
			{
				file << args[0] << " " << args[1] << " " << args[2] << " " << args[3] << " " <<new_seed << std::endl;
				args[0]++;
			}
		}
		alteration_index++;
		file.close();
		do_alter = true;
	}

	file.open(seed_profile.c_str(), std::ios::out);
	file.close();
	return true;
}

int Garden::GetSeedsProcessed()
{
	return seeds_processed;
}
#endif

/*
NOTES TO SELF:

-maybe create error message handling class or struct


*/
