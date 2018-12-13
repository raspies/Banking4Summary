#include <map>
#include <string>
#include <sstream>

struct expense
{
	int year;
	int month;
	double amount;
	std::string category;
	
	expense() : year(0), month(0), amount(0.0), category("")
	{
	}
};

#pragma once
class MonthExpense
{
public:
	MonthExpense(int year, int month) : m_year(year), m_month(month)
	{
	}
	
	~MonthExpense() {};

	int getYear() { return m_year; }
	int getMonth() { return m_month; }

	bool matches(int year, int month)
	{
		if (year == m_year && month == m_month)
			return true;
		else
			return false;
	}

	void addExpense(string category, double amount)
	{
		m_expenses[category] = m_expenses[category] + amount;
	}

	std::string getAmount(std::string category)
	{
		std::stringstream stream;

		std::locale mylocale("");   // get global locale
		stream.imbue(mylocale);  // imbue global locale
		stream << fixed << setprecision(2) << m_expenses[category];

		return stream.str();
	}

	int getNumberOfCategories() { return static_cast<int>(m_expenses.size()); }

	std::vector<string> getCategories()
	{
		std::vector<string> categories;
		for (auto element : m_expenses)
		{
			categories.push_back(element.first);
		}
		return categories;
	}

	std::string getHeaderOutputLine()
	{
		std::string output;

		for (auto element : m_expenses)
		{
			output.append(element.first);
			output.append(";");
		}

		return output;
	}

private:
	int m_year;
	int m_month;

	std::map<std::string, double> m_expenses;
};





