using System;
using System.IO;
using System.Text;

namespace BaiduBce.Util.Json;

internal class JsonLexer
{
	private StreamReader input;

	private int nextCharBuffer = -1;

	private int currentLine = 1;

	private int currentColumn;

	public JsonTokenType TokenType { get; set; }

	public object TokenValue { get; set; }

	public int TokenLine { get; set; }

	public int TokenColumn { get; set; }

	public JsonLexer(StreamReader input)
	{
		this.input = input;
	}

	public void ReadNextToken()
	{
		while (true)
		{
			int num = ReadNextChar();
			switch (num)
			{
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 32:
				continue;
			}
			TokenLine = currentLine;
			TokenColumn = currentColumn;
			switch (num)
			{
			case -1:
				TokenType = JsonTokenType.End;
				return;
			case 123:
				TokenType = JsonTokenType.OpeningBrace;
				return;
			case 125:
				TokenType = JsonTokenType.ClosingBrace;
				return;
			case 91:
				TokenType = JsonTokenType.OpeningBracket;
				return;
			case 93:
				TokenType = JsonTokenType.ClosingBracket;
				return;
			case 58:
				TokenType = JsonTokenType.Colon;
				return;
			case 44:
				TokenType = JsonTokenType.Comma;
				return;
			case 34:
				TokenType = JsonTokenType.String;
				TokenValue = ReadJsonString();
				return;
			case 116:
				ReadTrue();
				TokenType = JsonTokenType.Bool;
				TokenValue = true;
				return;
			case 102:
				ReadFalse();
				TokenType = JsonTokenType.Bool;
				TokenValue = false;
				return;
			case 110:
				ReadNull();
				TokenType = JsonTokenType.Null;
				return;
			}
			switch (num)
			{
			case 45:
			case 48:
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			{
				StringBuilder stringBuilder = new StringBuilder(128);
				stringBuilder.Append(num);
				ReadNumber(stringBuilder);
				break;
			}
			}
			return;
		}
	}

	public string ReadJsonString()
	{
		StringBuilder stringBuilder = new StringBuilder(128);
		stringBuilder.Append('"');
		while (true)
		{
			int num = ReadNextChar();
			stringBuilder.Append(num);
			if (num >= 0)
			{
				switch (num)
				{
				default:
					continue;
				case 10:
					break;
				case 34:
					return stringBuilder.ToString();
				case 92:
					stringBuilder.Append(ReadEscapedChar());
					continue;
				}
			}
			break;
		}
		throw new JsonParseException("Malformed Json: Unclosed string");
	}

	public char ReadEscapedChar()
	{
		return ReadNextChar() switch
		{
			34 => '"', 
			92 => '\\', 
			47 => '/', 
			98 => '\b', 
			102 => '\f', 
			110 => '\n', 
			114 => '\r', 
			116 => '\t', 
			117 => ReadEspacedUnicodeChar(), 
			_ => throw new JsonParseException("Malformed Json: Invalid escaped sequence."), 
		};
	}

	public char ReadEspacedUnicodeChar()
	{
		int num = 0;
		for (int i = 0; i < 4; i++)
		{
			int num2 = ReadNextChar();
			num *= 16;
			if (num2 >= 48 && num2 <= 57)
			{
				num += num2 - 48;
				continue;
			}
			if (num2 >= 65 && num2 <= 70)
			{
				num += num2 - 65 + 10;
				continue;
			}
			if (num2 >= 97 && num2 <= 102)
			{
				num += num2 - 97 + 10;
				continue;
			}
			throw new JsonParseException("Malformed Json: Invalid escpaed unicode sequence.");
		}
		return (char)num;
	}

	public void ReadNumber(StringBuilder builder)
	{
		while (true)
		{
			int num = ReadNextChar();
			if (num == 46 || num == 101 || num == 69)
			{
				builder.Append(num);
				ReadDouble(builder);
			}
			if (num < 48 || num > 57)
			{
				PutBackChar(num);
				TokenType = JsonTokenType.Long;
				try
				{
					TokenValue = long.Parse(builder.ToString());
				}
				catch (FormatException innerException)
				{
					throw new JsonParseException("Malformed Json: Fail to parse number.", innerException);
				}
				catch (OverflowException innerException2)
				{
					throw new JsonParseException("Malformed Json: Number too large.", innerException2);
				}
			}
			builder.Append(num);
		}
	}

	public void ReadDouble(StringBuilder builder)
	{
		int num = ReadNextChar();
		switch (num)
		{
		case 43:
		case 45:
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
			builder.Append(num);
			do
			{
				num = ReadNextChar();
			}
			while (num >= 48 && num <= 57);
			break;
		}
		TokenType = JsonTokenType.Double;
		try
		{
			TokenValue = double.Parse(builder.ToString());
		}
		catch (FormatException innerException)
		{
			throw new JsonParseException("Malformed Json: Fail to parse number.", innerException);
		}
		catch (OverflowException innerException2)
		{
			throw new JsonParseException("Malformed Json: Number too large.", innerException2);
		}
		PutBackChar(num);
	}

	public void ReadTrue()
	{
		if (ReadNextChar() != 114 || ReadNextChar() != 117 || ReadNextChar() != 101)
		{
			throw new JsonParseException("Malformed Json: Unrecognized token, 'true' expected.");
		}
	}

	public void ReadFalse()
	{
		if (ReadNextChar() != 97 || ReadNextChar() != 108 || ReadNextChar() != 115 || ReadNextChar() != 101)
		{
			throw new JsonParseException("Malformed Json: Unrecognized token, 'false' expected.");
		}
	}

	public void ReadNull()
	{
		if (ReadNextChar() != 117 || ReadNextChar() != 108 || ReadNextChar() != 108)
		{
			throw new JsonParseException("Malformed Json: Unrecognized token, 'null' expected.");
		}
	}

	private int ReadNextChar()
	{
		int num;
		if (nextCharBuffer >= 0)
		{
			num = nextCharBuffer;
			nextCharBuffer = -1;
		}
		else
		{
			while (true)
			{
				num = input.Read();
				if (num != 10 && num != 13)
				{
					break;
				}
				currentLine++;
				currentColumn = 0;
			}
		}
		currentColumn++;
		return num;
	}

	private void PutBackChar(int ch)
	{
		nextCharBuffer = ch;
		currentColumn--;
	}
}
