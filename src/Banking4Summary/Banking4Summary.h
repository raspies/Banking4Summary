#pragma once

void writeMonthExpensesToFile(std::vector<MonthExpense> &monthExpenses, std::string outputFilename);


static int getIndexOfColumn(const std::vector<std::string> &headers, string keyword)
{
	auto itBuchungsdatum = std::find(headers.begin(), headers.end(), keyword);

	if (itBuchungsdatum != headers.end())
	{
		return (itBuchungsdatum - headers.begin());
	}
	else
	{
		std::cout << "Column " << keyword << " not found in header line" << std::endl;
	}
	return -1;
}


static std::vector<std::string> split(const string& str, const string& delim)
{
	vector<string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == string::npos) pos = str.length();
		string token = str.substr(prev, pos - prev);
		tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}