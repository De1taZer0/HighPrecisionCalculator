#include <HighPrecision.hh>

namespace HighPrecisionCalculator
{

	const int Precision = 100; //计算精度

	HighPrecisionNumber::HighPrecisionNumber()
	{
		this->integerDigits.emplace_back(0);
		this->isNegative = false;
	}

	HighPrecisionNumber::HighPrecisionNumber(const int Input)
	{
		*this = HighPrecisionNumber(std::to_string(Input));
	}

	HighPrecisionNumber::HighPrecisionNumber(const double Input)
	{
		*this = HighPrecisionNumber(std::to_string(Input));
	}

	HighPrecisionNumber::HighPrecisionNumber(const long long Input)
	{
		*this = HighPrecisionNumber(std::to_string(Input));
	}

	HighPrecisionNumber::HighPrecisionNumber(const std::string& Input)
	{
		this->isNegative = false;
		if (Input.find('.') == std::string::npos)
		{
			for (std::string::const_reverse_iterator criter = Input.crbegin(); criter != Input.crend(); ++criter)
			{
				if (criter == Input.crend() - 1 && (*criter == '+' || *criter == '-'))
					this->isNegative = (*criter == '-');
				else
					this->integerDigits.emplace_back(*criter - '0');
			}
		}
		else
		{
			std::string::const_reverse_iterator criter = Input.crbegin();
			do
				this->decimalDigits.emplace_back(*criter - '0');
			while (*(++criter) != '.');
			while ((++criter) != Input.crend())
			{
				if (criter == Input.crend() - 1 && (*criter == '+' || *criter == '-'))
					this->isNegative = *criter == '-';
				else
					this->integerDigits.emplace_back(*criter - '0');
			}
		}
		this->trim();
	}

	HighPrecisionNumber::HighPrecisionNumber(const unsigned long long Input)
	{
		*this = HighPrecisionNumber(std::to_string(Input));
	}

	bool HighPrecisionNumber::IsOdd()const
	{
		if (this->decimalDigits.empty())
			return *integerDigits.begin() & 1;
		return false;
	}

	HighPrecisionNumber HighPrecisionNumber::floor()const
	{
		HighPrecisionNumber res = *this;
		if (!res.decimalDigits.empty() && res.isNegative)
			res -= 1;
		res.decimalDigits.clear();
		return res;
	}

	HighPrecisionNumber HighPrecisionNumber::deldec()const
	{
		HighPrecisionNumber res = *this;
		res.decimalDigits.clear();
		return res;
	}

	HighPrecisionNumber HighPrecisionNumber::factorial()const
	{
		HighPrecisionNumber res(1);
		for (HighPrecisionNumber i(1); i <= *this; i += 1)
			res *= i;
		res.trim();
		return res;
	}

	HighPrecisionNumber operator - (const HighPrecisionNumber& a)
	{
		HighPrecisionNumber res = a;
		res.isNegative = !res.isNegative;
		return res;
	}

	HighPrecisionNumber operator + (const HighPrecisionNumber& ta, const HighPrecisionNumber& tb)
	{
		if (ta.isNegative && !tb.isNegative)
			return tb - (-ta);
		if (!ta.isNegative && tb.isNegative)
			return ta - (-tb);
		if (ta.isNegative && tb.isNegative)
			return -((-ta) + (-tb));
		int movval = (ta.decimalDigits.size() > tb.decimalDigits.size()) ? ta.decimalDigits.size() : tb.decimalDigits.size();
		HighPrecisionNumber res, a(ta << movval), b(tb << movval);
		res.integerDigits.clear();
		bool tempcarr = 0;
		std::vector<char>::const_iterator iterai = a.integerDigits.cbegin();
		std::vector<char>::const_iterator iterbi = b.integerDigits.cbegin();
		while (iterai != a.integerDigits.cend() || iterbi != b.integerDigits.cend())
		{
			if (iterai == a.integerDigits.cend())
			{
				res.integerDigits.emplace_back((*(iterbi)+tempcarr) % 10);
				tempcarr = (*(iterbi++) + tempcarr) / 10;
				continue;
			}
			if (iterbi == b.integerDigits.cend())
			{
				res.integerDigits.emplace_back((*(iterai)+tempcarr) % 10);
				tempcarr = (*(iterai++) + tempcarr) / 10;
				continue;
			}
			res.integerDigits.emplace_back((*iterai + *iterbi + tempcarr) % 10);
			tempcarr = (*(iterai++) + *(iterbi++) + tempcarr) / 10;
		}
		res.integerDigits.emplace_back(tempcarr);
		res >>= movval;
		res.trim();
		return res;
	}

	HighPrecisionNumber operator - (const HighPrecisionNumber& ta, const HighPrecisionNumber& tb)
	{
		if (ta.isNegative && tb.isNegative)
			return (-tb) - (-ta);
		if (!ta.isNegative && tb.isNegative)
			return ta + (-tb);
		if (ta.isNegative && !tb.isNegative)
			return -((-ta) + tb);
		if (ta < tb)
			return -(tb - ta);
		HighPrecisionNumber res, a(ta), b(tb);
		res.integerDigits.clear();
		bool tempborr = 0;
		int movval = (a.decimalDigits.size() > b.decimalDigits.size()) ? a.decimalDigits.size() : b.decimalDigits.size();
		a <<= movval;
		b <<= movval;
		std::vector<char>::const_iterator iterai = a.integerDigits.cbegin();
		std::vector<char>::const_iterator iterbi = b.integerDigits.cbegin();
		while (iterai != a.integerDigits.cend() || iterbi != b.integerDigits.cend())
		{
			if (iterbi == b.integerDigits.cend())
			{
				res.integerDigits.emplace_back(10 * (*iterai < tempborr) + *iterai - tempborr);
				tempborr = (*iterai < tempborr);
				++iterai;
				continue;
			}
			res.integerDigits.emplace_back(10 * (*iterai < *iterbi + tempborr) + *iterai - *iterbi - tempborr);
			tempborr = (*(iterai++) < *(iterbi++) + tempborr);
		}
		res >>= movval;
		res.trim();
		return res;
	}

	HighPrecisionNumber operator * (const HighPrecisionNumber& ta, const HighPrecisionNumber& tb)
	{
		if (ta == 0 || tb == 0)
			return HighPrecisionNumber(0);
		HighPrecisionNumber res, tempnum, a = ta << ta.decimalDigits.size(), b = tb << tb.decimalDigits.size();
		int cnt = 0, tempcarr = 0;
		for (std::vector<char>::const_iterator iterai = a.integerDigits.cbegin(); iterai != a.integerDigits.cend(); ++iterai)
		{
			tempnum.integerDigits.clear();
			for (std::vector<char>::const_iterator iterbi = b.integerDigits.cbegin(); iterbi != b.integerDigits.cend(); ++iterbi)
			{
				tempnum.integerDigits.emplace_back(((*iterai) * (*iterbi) + tempcarr) % 10);
				tempcarr = ((*iterai) * (*iterbi) + tempcarr) / 10;
			}
			while (tempcarr)
			{
				tempnum.integerDigits.emplace_back(tempcarr % 10);
				tempcarr /= 10;
			}
			tempnum.integerDigits.insert(tempnum.integerDigits.begin(), cnt, 0);
			res += tempnum;
			++cnt;
		}
		res.isNegative = a.isNegative ^ b.isNegative;
		res >>= int(ta.decimalDigits.size() + tb.decimalDigits.size());
		res.trim();
		return res;
	}

	HighPrecisionNumber operator / (const HighPrecisionNumber& ta, const HighPrecisionNumber& tb)
	{
		if (ta == 0 || tb == 0)
			return HighPrecisionNumber(0);
		if (ta == 1)
			return reciprocal(tb);
		if (tb == 1)
			return ta;
		else
			return ta * reciprocal(tb);
	}

	HighPrecisionNumber operator % (const HighPrecisionNumber& a, const HighPrecisionNumber& b)
	{
		if (!a.decimalDigits.empty() || !b.decimalDigits.empty())           // 小数取余时返回 b 表示出错
			return b;
		if (a < b && !a.isNegative)
			return a;
		HighPrecisionNumber res = a - ((a / b).deldec() * b);
		res.trim();
		return res;
	}

	HighPrecisionNumber operator ^ (const HighPrecisionNumber& a, const HighPrecisionNumber& b)
	{
		if (a.isNegative && !b.decimalDigits.empty())
			return HighPrecisionNumber(0);
		if (a == 0)
			return HighPrecisionNumber(0);
		if (a == 1 || b == 0)
			return HighPrecisionNumber(1);
		if (b == 1)
			return a;
		if (b.isNegative)
			return reciprocal((a ^ (-b)));
		HighPrecisionNumber res;
		if (b.decimalDigits.empty())
		{
			res = FastPow(a, b);
		}
		else
		{
			if (a.isNegative)
			{
				res = pow10(b * lg(-a));
				res.isNegative = b.IsOdd();
			}
			else
				res = pow10(b * lg(a));
		}
		return res;
	}

	template<typename T> HighPrecisionNumber operator + (const HighPrecisionNumber& a, const T& b)
	{
		return a + HighPrecisionNumber(b);
	}

	template<typename T> HighPrecisionNumber operator - (const HighPrecisionNumber& a, const T& b)
	{
		return a - HighPrecisionNumber(b);
	}

	template<typename T> HighPrecisionNumber operator * (const HighPrecisionNumber& a, const T& b)
	{
		return a * HighPrecisionNumber(b);
	}

	template<typename T> HighPrecisionNumber operator / (const HighPrecisionNumber& a, const T& b)
	{
		return a / HighPrecisionNumber(b);
	}

	template<typename T> HighPrecisionNumber operator % (const HighPrecisionNumber& a, const T& b)
	{
		return a % HighPrecisionNumber(b);
	}

	template<typename T> HighPrecisionNumber operator ^ (const HighPrecisionNumber& a, const T& b)
	{
		return a ^ HighPrecisionNumber(b);
	}


	template<typename T> HighPrecisionNumber operator + (const T& a, const HighPrecisionNumber& b)
	{
		return HighPrecisionNumber(a) + b;
	}

	template<typename T> HighPrecisionNumber operator - (const T& a, const HighPrecisionNumber& b)
	{
		return HighPrecisionNumber(a) - b;
	}

	template<typename T> HighPrecisionNumber operator * (const T& a, const HighPrecisionNumber& b)
	{
		return HighPrecisionNumber(a) * b;
	}

	template<typename T> HighPrecisionNumber operator / (const T& a, const HighPrecisionNumber& b)
	{
		return HighPrecisionNumber(a) / b;
	}

	template<typename T> HighPrecisionNumber operator % (const T& a, const HighPrecisionNumber& b)
	{
		return HighPrecisionNumber(a) % b;
	}

	template<typename T> HighPrecisionNumber operator ^ (const T& a, const HighPrecisionNumber& b)
	{
		return HighPrecisionNumber(a) ^ b;
	}


	HighPrecisionNumber operator << (const HighPrecisionNumber& a, const int t)          //十进制左移
	{
		if (t < 0)
			return a >> (-t);
		if (t == 0)
			return a;
		HighPrecisionNumber res(a);
		int b = t;
		while (!res.decimalDigits.empty() && b)
		{
			res.integerDigits.insert(res.integerDigits.begin(), res.decimalDigits.at(res.decimalDigits.size() - 1));
			res.decimalDigits.pop_back();
			--b;
		}
		res.integerDigits.insert(res.integerDigits.begin(), b, 0);
		res.trim();
		return res;
	}

	HighPrecisionNumber operator >> (const HighPrecisionNumber& a, const int t)          //十进制右移
	{
		if (t < 0)
			return a << (-t);
		if (t == 0)
			return a;
		HighPrecisionNumber res(a);
		int b = t;
		while (!res.integerDigits.empty() && b)
		{
			res.decimalDigits.emplace_back(res.integerDigits.at(0));
			res.integerDigits.erase(res.integerDigits.begin());
			--b;
		}
		res.decimalDigits.insert(res.decimalDigits.end(), b, 0);
		res.trim();
		return res;
	}



	template<typename T> HighPrecisionNumber HighPrecisionNumber::operator = (const T& t)
	{
		return HighPrecisionNumber(std::to_string(t));
	}

	template<typename T> HighPrecisionNumber& operator += (HighPrecisionNumber& a, const T& b)
	{
		a = a + HighPrecisionNumber(b);
		return a;
	}

	template<typename T> HighPrecisionNumber& operator -= (HighPrecisionNumber& a, const T& b)
	{
		a = a - HighPrecisionNumber(b);
		return a;
	}

	template<typename T> HighPrecisionNumber& operator *= (HighPrecisionNumber& a, const T& b)
	{
		a = a * HighPrecisionNumber(b);
		return a;
	}

	template<typename T> HighPrecisionNumber& operator /= (HighPrecisionNumber& a, const T& b)
	{
		a = a / HighPrecisionNumber(b);
		return a;
	}

	template<typename T> HighPrecisionNumber& operator %= (HighPrecisionNumber& a, const T& b)
	{
		a = a % HighPrecisionNumber(b);
		return a;
	}

	template<typename T> HighPrecisionNumber& operator ^= (HighPrecisionNumber& a, const T& b)
	{
		a = a ^ HighPrecisionNumber(b);
		return a;
	}

	HighPrecisionNumber& operator <<= (HighPrecisionNumber& a, const int b)
	{
		a = a << b;
		return a;
	}

	HighPrecisionNumber& operator >>= (HighPrecisionNumber& a, const int b)
	{
		a = a >> b;
		return a;
	}

	//逻辑运算
	bool operator ! (const HighPrecisionNumber& a)
	{
		if (a == 0)
			return true;
		return false;
	}

	bool operator == (const HighPrecisionNumber& a, const HighPrecisionNumber& b)
	{
		if (a.isNegative ^ b.isNegative)
			return false;
		return (a.integerDigits == b.integerDigits) && (a.decimalDigits == b.decimalDigits);
	}

	bool operator != (const HighPrecisionNumber& a, const HighPrecisionNumber& b)
	{
		return !(a == b);
	}

	bool operator > (const HighPrecisionNumber& a, const HighPrecisionNumber& b)
	{
		if (a == b)
			return false;
		if (a.isNegative && b.isNegative)
			return ((-a) < (-b));
		if (a.isNegative && !b.isNegative)
			return false;
		if (!a.isNegative && b.isNegative)
			return true;
		if (a.integerDigits.size() > b.integerDigits.size())
			return true;
		if (a.integerDigits.size() < b.integerDigits.size())
			return false;
		std::vector<char>::const_reverse_iterator iterai = a.integerDigits.crbegin();
		std::vector<char>::const_reverse_iterator iterbi = b.integerDigits.crbegin();
		do
		{
			if (*iterai > *iterbi)
				return true;
			if (*iterai < *iterbi)
				return false;
		} while ((++iterai) != a.integerDigits.crend() && (++iterbi) != b.integerDigits.crend());
		if (!a.decimalDigits.empty() && b.decimalDigits.empty())
			return true;
		if (a.decimalDigits.empty() && !b.decimalDigits.empty())
			return false;
		std::vector<char>::const_reverse_iterator iterad = a.decimalDigits.crbegin();
		std::vector<char>::const_reverse_iterator iterbd = b.decimalDigits.crbegin();
		do
		{
			if (*iterad > *iterbd)
				return true;
			if (*iterad < *iterbd)
				return false;
		} while ((++iterad) != a.decimalDigits.crend() && (++iterbd) != b.decimalDigits.crend());
		if (iterbd == b.decimalDigits.crend())
			return true;
		return false;
	}

	bool operator < (const HighPrecisionNumber& a, const HighPrecisionNumber& b)
	{
		return !(a >= b);
	}

	bool operator >= (const HighPrecisionNumber& a, const HighPrecisionNumber& b)
	{
		return (a > b) || (a == b);
	}

	bool operator <= (const HighPrecisionNumber& a, const HighPrecisionNumber& b)
	{
		return !(a > b);
	}

	template<typename T> bool operator == (const HighPrecisionNumber& a, const T& b)
	{
		return a == HighPrecisionNumber(b);
	}

	template<typename T> bool operator != (const HighPrecisionNumber& a, const T& b)
	{
		return a != HighPrecisionNumber(b);
	}

	template<typename T> bool operator > (const HighPrecisionNumber& a, const T& b)
	{
		return a > HighPrecisionNumber(b);
	}

	template<typename T> bool operator < (const HighPrecisionNumber& a, const T& b)
	{
		return a < HighPrecisionNumber(b);
	}

	template<typename T> bool operator >= (const HighPrecisionNumber& a, const T& b)
	{
		return a >= HighPrecisionNumber(b);
	}

	template<typename T> bool operator <= (const HighPrecisionNumber& a, const T& b)
	{
		return a <= HighPrecisionNumber(b);
	}

	template<typename T> bool operator == (const T& b, const HighPrecisionNumber& a)
	{
		return HighPrecisionNumber(a) == b;
	}

	template<typename T> bool operator != (const T& b, const HighPrecisionNumber& a)
	{
		return HighPrecisionNumber(a) != b;
	}

	template<typename T> bool operator > (const T& b, const HighPrecisionNumber& a)
	{
		return HighPrecisionNumber(a) > b;
	}

	template<typename T> bool operator < (const T& b, const HighPrecisionNumber& a)
	{
		return HighPrecisionNumber(a) < b;
	}

	template<typename T> bool operator >= (const T& b, const HighPrecisionNumber& a)
	{
		return HighPrecisionNumber(a) >= b;
	}

	template<typename T> bool operator <= (const T& b, const HighPrecisionNumber& a)
	{
		return HighPrecisionNumber(a) <= b;
	}

	//流IO
	std::ostream& operator << (std::ostream& output, const HighPrecisionNumber& a)
	{
		if (a.isNegative)
			output << "-";
		for (std::vector<char>::const_reverse_iterator iter = a.integerDigits.crbegin(); iter != a.integerDigits.rend(); ++iter)
			output << (int)*iter;
		if (!a.decimalDigits.empty())
		{
			output << '.';
			for (std::vector<char>::const_reverse_iterator iter = a.decimalDigits.crbegin(); iter != a.decimalDigits.crend(); ++iter)
				output << (int)*iter;
		}
		return output;
	}

	std::istream& operator >> (std::istream& input, HighPrecisionNumber& a)
	{
		std::string inputStr;
		input >> inputStr;
		a = HighPrecisionNumber(inputStr);
		return input;
	}



	void HighPrecisionNumber::trim()
	{
		if (!this->decimalDigits.empty())
		{
			std::vector<char>::const_iterator iterd = this->decimalDigits.cbegin();
			while (iterd != this->decimalDigits.cend() && !*iterd)
				iterd = this->decimalDigits.erase(iterd);
			while (this->decimalDigits.size() > (Precision * 4))
				this->decimalDigits.erase(this->decimalDigits.cbegin());
		}
		std::vector<char>::const_reverse_iterator iteri = this->integerDigits.crbegin();
		while (iteri != this->integerDigits.crend() && !*iteri)
			iteri = std::vector<char>::const_reverse_iterator(this->integerDigits.erase((++iteri).base()));
		if (this->integerDigits.empty() && this->decimalDigits.empty())
			this->isNegative = false;
		if (this->integerDigits.empty())
			this->integerDigits.emplace_back(0);
		return;
	}


	HighPrecisionNumber reciprocal(const HighPrecisionNumber& a)
	{
		if (a.decimalDigits.empty())
		{
			if (a == 2)
				return HighPrecisionNumber(0.5);
			if (a == 5)
				return HighPrecisionNumber(0.2);
			if ((a / 2).decimalDigits.empty())
				return 0.5 * reciprocal(a / 2);
			if ((a / 5).decimalDigits.empty())
				return 0.2 * reciprocal(a / 5);
		}
		HighPrecisionNumber res = (HighPrecisionNumber(1) >> (a.decimalDigits.size() + a.integerDigits.size() + 1));
		HighPrecisionNumber last;
		do
		{
			last = res;
			res *= (2 - (a * res));
		} while ((res - last) > ((HighPrecisionNumber(1) >> (Precision * 2))) || (res - last) < ((HighPrecisionNumber(-1) >> (Precision * 2))));
		if ((res + (HighPrecisionNumber(1) >> (res.decimalDigits.size()))) * a == 1)
			res += (HighPrecisionNumber(1) >> (res.decimalDigits.size()));
		return res;
	}


	HighPrecisionNumber lg(const HighPrecisionNumber& a)
	{
		HighPrecisionNumber val(a), res, tmp(1);
		int k = 0, s = 0;
		while (val > 1)
		{
			val >>= 1;
			++k;
		}
		while (val < 1)
		{
			val <<= 1;
			--k;
		}
		if (val == 1)
			return HighPrecisionNumber(k);
		while (val > 1)
		{
			val *= 0.5;
			++s;
		}
		if (val == 1)
			return k + (lg2 * s);
		val -= 1;
		for (int i = 1; i <= Precision; ++i)
		{
			tmp *= val;
			res += ((i & 1) ? 1 : -1) * tmp * reciprocal(HighPrecisionNumber(i));
		}
		return k + (reciln10 * res) + (lg2 * s);
	}

	HighPrecisionNumber pow10(const HighPrecisionNumber& a)
	{
		if (a < 0)
			return reciprocal(pow10(-a));
		if (a.decimalDigits.empty())
			return FastPow(HighPrecisionNumber(10), a);
		HighPrecisionNumber tempa = a * ln10;
		HighPrecisionNumber val((tempa - tempa.deldec())), res(0), tmp(1);
		for (int i = 1; i <= Precision; ++i)
		{
			res += tmp;
			tmp *= val * reciprocal(HighPrecisionNumber(i));
		}
		return (res * FastPow(e, tempa.deldec()));
	}

	HighPrecisionNumber FastPow(const HighPrecisionNumber& a, const HighPrecisionNumber& b)
	{
		HighPrecisionNumber res(1), n(b), temp(a);
		res.isNegative = a.isNegative && n.IsOdd();
		while (n != 0)
		{
			if (n.IsOdd())
				res *= temp;
			temp *= temp;
			n /= 2;
			n.decimalDigits.clear();
		}
		return res;
	}






	std::map<std::string, int> operationOrder = { {"(",1},{"+",2},{"-",2},{"*",3},{"/",3},{"%",3},{"^",4},{"!",4},{")",5} };

	HighPrecisionExpression::HighPrecisionExpression()
	{
		this->expressionstr.clear();
		while (!this->constant.empty())
			this->constant.pop();
		while (!this->operation.empty())
			this->operation.pop();
	}

	HighPrecisionExpression::HighPrecisionExpression(const std::string& Input)
	{
		this->expressionstr = Input;
	}

	bool HighPrecisionExpression::calculate()
	{
		std::string::const_iterator iter = this->expressionstr.cbegin();
		std::string num, oper;
		while (iter != this->expressionstr.cend())
		{
			if (*iter == '+' || *iter == '-')
			{
				num += *iter;
			}
			while (iter != this->expressionstr.cend() && ((*iter >= '0' && *iter <= '9') || *iter == '.'))
			{
				num += *iter;
				++iter;
			}
			if (!num.empty())
			{
				this->constant.push(HighPrecisionNumber(num));
				num.clear();
			}
			while (iter != this->expressionstr.cend() && (*iter < '0' || *iter > '9'))
			{
				while (iter != this->expressionstr.cend() && (*iter < '0' || *iter > '9'))
				{
					oper += *iter;
					++iter;
					if (operationOrder[oper])
						break;
				}
				if (!oper.empty())
				{
					if (!operationOrder[oper])
						return false;
					if (oper == "!")
					{
						if (this->constant.empty())
							return false;

					}
					if (oper == "(" || this->operation.empty())
						this->operation.push(oper);
					else if (oper == ")")
					{
						while (this->operation.top() != "(")
						{
							if (!this->cal(this->operation.top()))
								return false;
							this->operation.pop();
						}
						this->operation.pop();
					}
					else if (!oper.empty())
					{
						while (!this->operation.empty() && (operationOrder[oper] <= operationOrder[this->operation.top()]))
						{
							if (!this->cal(this->operation.top()))
								return false;
							this->operation.pop();
						}
						this->operation.push(oper);
					}
					oper.clear();
				}
			
			}
		}
		while (!this->operation.empty() && !this->constant.empty())
		{
			if (!this->cal(this->operation.top()))
				return false;
			this->operation.pop();
		}
		if (!this->operation.empty())
			return false;
		return true;

	}

	bool operator == (const HighPrecisionExpression& a, const std::string& b)
	{
		return a.expressionstr == b;
	}

	bool operator != (const HighPrecisionExpression& a, const std::string& b)
	{
		return a.expressionstr != b;
	}

	bool operator == (const std::string& a, const HighPrecisionExpression& b)
	{
		return a == b.expressionstr;
	}

	bool operator != (const std::string& a, const HighPrecisionExpression& b)
	{
		return a != b.expressionstr;
	}



	std::ostream& operator << (std::ostream& output, const HighPrecisionExpression& a)
	{
		output << a.constant.top();
		return output;
	}

	std::istream& operator >> (std::istream& input, HighPrecisionExpression& a)
	{
		input >> a.expressionstr;
		return input;
	}

	bool HighPrecisionExpression::cal(const std::string& oper)
	{
		if (oper == "!")
		{
			HighPrecisionNumber A;
			A = this->constant.top();
			this->constant.pop();
			this->constant.push(A.factorial());
			return true;
		}
		HighPrecisionNumber A, B, res;
		A = this->constant.top();
		this->constant.pop();
		if (this->constant.empty())
			return false;
		B = this->constant.top();
		this->constant.pop();
		if (oper == "+")
			res = B + A;
		else if (oper == "-")
			res = B - A;
		else if (oper == "*")
			res = B * A;
		else if (oper == "/")
		{
			if (A == 0)
				return false;
			res = B / A;
		}
		else if (oper == "%")
		{
			if (A == 0)
				return false;
			res = B % A;
			if (res == A)
				return false;
		}
		else if (oper == "^")
			res = B ^ A;
		this->constant.push(res);
		return true;

	}

	void HighPrecisionExpression::clear()
	{
		while (!this->constant.empty())
			this->constant.pop();
		while (!this->operation.empty())
			this->operation.pop();
		this->expressionstr.clear();
		return;
	}



}