// Banking4Summary.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

#include "MonthExpense.h"
#include "Banking4Summary.h"
#include "cxxopts.h"

const std::string ID_Buchungsdatum = "Buchungsdatum";
const std::string ID_Kategorie = "Kategorie";
const std::string ID_Betrag = "Betrag";

void addExpense(std::vector<MonthExpense>& monthExpenses, expense newExpense)
{
	bool monthWasFound = false;
	for (auto &expense : monthExpenses)
	{
		if (expense.matches(newExpense.year, newExpense.month))
		{
			expense.addExpense(newExpense.category, newExpense.amount);
			monthWasFound = true;
			return;
		}
	}
	if (!monthWasFound)
	{
		MonthExpense newMonth(newExpense.year, newExpense.month);
		newMonth.addExpense(newExpense.category, newExpense.amount);
		monthExpenses.push_back(newMonth);
	}
}

void parseFile(std::string filename, std::vector<MonthExpense>& monthExpenses, bool exportOnlyMainCategories = true)
{
	std::ifstream ifs(filename);

	std::string line;
	std::vector<std::string> headers;

	//get headerline
	if (std::getline(ifs, line))
	{
		if (!line.empty())
		{
			headers = split(line, ";");
		}
	}
	int indexBuchungsdatum = getIndexOfColumn(headers, ID_Buchungsdatum);
	int indexKategorie = getIndexOfColumn(headers, ID_Kategorie);
	int indexBetrag = getIndexOfColumn(headers, ID_Betrag);

	if (indexBetrag == -1 || indexKategorie == -1 || indexBuchungsdatum == -1)
	{
		return;
	}

	//read elements
	while (std::getline(ifs, line))
	{
		expense newExpense;

		//split to columns
		vector<string> entryRawLine = split(line, ";");

		//get month and year of current line
		vector<string> dateSplit = split(entryRawLine[indexBuchungsdatum], ".");
		newExpense.month = std::stoi(dateSplit[1]);
		newExpense.year = std::stoi(dateSplit[2]);
		newExpense.amount = std::stod(entryRawLine[indexBetrag]);
		newExpense.category = entryRawLine[indexKategorie];

		if (exportOnlyMainCategories)
		{
			size_t pos = newExpense.category.find(":");
			if (pos != std::string::npos)
			{
				newExpense.category = newExpense.category.substr(0, pos);
			}
		}
		addExpense(monthExpenses, newExpense);

	}

	return;
}


void writeMonthExpensesToFile(std::vector<MonthExpense> &monthExpenses, std::string outputFilename)
{
	std::ofstream outputFile;

	outputFile.open(outputFilename, std::ofstream::out | std::ofstream::trunc);

	if (!outputFile.is_open())
	{
		std::cerr << "Could not open output file " << std::endl;
		return;
	}

	//find all used categories
	std::vector<string> categories;
	for (auto &monthExpense : monthExpenses)
	{
		std::vector<string> newCategories = monthExpense.getCategories();

		//add all new categories to overall vector
		for (auto category : newCategories)
		{
			if (category.empty())
			{
				category = "(Nicht gesetzt)";
			}
			
			if (std::find(categories.begin(), categories.end(), category) == categories.end())
			{	
				categories.push_back(category);
			}
		}
	}
	//sort categories alphabetically
	std::sort(categories.begin(), categories.end());

	//add header line to file
	outputFile << "Monat;";
	for (auto category : categories)
	{
		outputFile << category << ";";
	}
	outputFile << std::endl;

	// add all month expenses to file
	for (auto &monthExpense : monthExpenses)
	{
		//add month as text
		std::stringstream ss;
		ss << monthExpense.getYear() << "/" << std::setw(2) << std::setfill('0') << monthExpense.getMonth() << ";";
		outputFile << ss.str();

		for (auto category : categories)
		{
			outputFile << monthExpense.getAmount(category) << ";";
		}
		outputFile << std::endl;
	}

	outputFile.close();
}

int main(int argc, char* argv[])
{
	//
	cxxopts::Options options(argv[0], " banking4 Monthly Summary \n 1. Open banking4W and open account \n 2. Click 'Extras->Export' \n 3. Select bank account, \n 4. Select format 'Outbank CSV' and export data \n 5. Export each account you want to include in the summary to a file, multiple files can be parsed together \n 6. Run Banking4Summary.exe with proper arguments");
	options
		.positional_help("[optional args]")
		.show_positional_help();

	std::vector<std::string> csvFiles;

	options
		.allow_unrecognised_options()
		.add_options()
		("h, help", "Print help")
		("f, files", "CSV - Files", cxxopts::value<std::vector<std::string>>(), "")
		("d, dir", "Directory with files", cxxopts::value<std::string>()->default_value("./"), "")
		("m, mode", "Mode: Directory 'dir' or File 'file' ",
			cxxopts::value<std::string>()->default_value("file"), "")
		("o, output", "Output File name", cxxopts::value<std::string>()->default_value("output.csv"),"")
		("c, categories", "Categories: 'main' for only main categories, 'sub' for all sub-categories",
			cxxopts::value<std::string>()->default_value("main"), "");

	cxxopts::ParseResult result = options.parse(argc, argv);

	try
	{
		if (result.count("help"))
		{
			std::cout << options.help() << std::endl;
			exit(0);
		}

		if (result.count("m"))
		{
			std::string modeStr = result["m"].as<std::string>();

			if (modeStr.compare("file") == 0)
			{
				if (!result.count("f"))
				{
					std::cout << "Error: Mode 'file' was set, but no file was given" << std::endl;
					exit(-1);
				}
				else
				{
					csvFiles = result["f"].as<std::vector<std::string>>();

					std::cout << "Selected file are: ";
					for (auto &p : csvFiles)
					{
						std::cout << p;
					}

					std::cout << std::endl;

				}
			}
			else
			{
				std::string path = result["d"].as<std::string>();

				std::cout << "Selected directory is: " << result["d"].as<std::string>() << std::endl;

				for (auto & p : fs::directory_iterator(path))
				{
					if (p.path().filename().string().find(".csv") != std::string::npos)
					{
						csvFiles.push_back(p.path().string());
						std::cout << "Found file: " << p.path().string() << std::endl;
					}
				}
			}
		}
	}
	catch (const cxxopts::OptionException& e)
	{
		std::cout << "error parsing options: " << e.what() << std::endl;
		exit(1);
	}


	bool exportOnlyMainCategories = true;
	if (result["c"].as<std::string>().compare("sub") == 0)
	{
		exportOnlyMainCategories = false;
		std::cout << "Exporting also sub categories" << std::endl;
	}

	std::vector<MonthExpense> monthExpenses;

	if (!csvFiles.empty())
	{
		for (auto file : csvFiles)
		{
			parseFile(file, monthExpenses, exportOnlyMainCategories);
		}

		//generate output file
		writeMonthExpensesToFile(monthExpenses, result["o"].as<std::string>());

		std::cout << "Output file: " << result["o"].as<std::string>() << " was generated" << std::endl;
	}
	
	return 0;
}

// Programm ausführen: STRG+F5 oder "Debuggen" > Menü "Ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.
