require 'test/unit'
require 'rejectu/rejectu'

class TestRejectu < Test::Unit::TestCase
  def test_valid_only_accepts_string
    assert_raises TypeError do
      Rejectu::valid?(Object.new)
    end
  end

  def test_valid_returns_boolean
    assert Rejectu::valid? "test string"
  end

  def test_invalid_string
    refute Rejectu::valid? "\xf2\xa4\xb7\xa4 test string"
  end

  def test_invalid_string2
    refute Rejectu::valid? "teststri\xf2\xa4\xb7\xa4"
  end

  def test_invalid_string3
    refute Rejectu::valid? "teststri12\xf2\xa4\xb7\xa4"
  end

  def test_non_utf8_string
    assert_raises ArgumentError do
      Rejectu::valid? "hello world".encode("ISO-8859-1")
    end
  end
end
