// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Types.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::files
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Collect all files in target directory. Optionaly do same for subdirectories.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	auto buildFileList ( str _TargetDir, bool _IncludeSubdirs = false ) -> std::vector<std::filesystem::path>
	{
		std::vector<std::filesystem::path> Files;
		std::vector<std::filesystem::path> Directories;

		for(auto& Entry : std::filesystem::directory_iterator(_TargetDir))
		{
			if(Entry.is_regular_file())	Files.push_back(Entry.path());
			if(Entry.is_directory() && _IncludeSubdirs) Directories.push_back(Entry.path());
		}

		if(_IncludeSubdirs)
		{
			for(auto& Directory : Directories)
			{
				auto DeepFile = buildFileList(Directory.string(), true); // Recursive scan.

				Files.insert(Files.end(), std::make_move_iterator(DeepFile.begin()), std::make_move_iterator(DeepFile.end())); // Append filenames.
			}
		}

		return Files; // Return filenames.
	}
	
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Load text file.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	auto loadText ( const str& _Filename ) -> str
	{
		auto File = std::ifstream(_Filename);

		if(File.is_open())
		{
			auto Text = str();
		
			while(!File.eof())
			{
				auto Line = str();
				std::getline(File, Line);

				Text += Line + str("\n");
			}

			return Text;
		}

		std::cout << "Error[fx->Files->loadText]\n";
		std::cout << "   Failed to open: " << _Filename << "!\n";
		return _Filename;
	}
}

