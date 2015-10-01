require 'test/unit'
require 'rejectu/rejectu'

class TestRejectu < Test::Unit::TestCase
  def test_valid_only_accepts_string
    assert_raises TypeError do
      Rejectu.valid?(Object.new)
    end
  end

  def test_valid_returns_boolean
    assert Rejectu.valid? "test string"
  end

  def test_valid_short_string
    assert Rejectu.valid? "test"
  end

  def test_invalid_string
    refute Rejectu.valid? "\xf2\xa4\xb7\xa4 test string"
  end

  def test_invalid_string2
    refute Rejectu.valid? "teststri\xf2\xa4\xb7\xa4"
  end

  def test_invalid_string3
    refute Rejectu.valid? "teststri12\xf2\xa4\xb7\xa4"
  end

  def test_non_utf8_string
    assert_raises ArgumentError do
      Rejectu.valid? "hello world".encode("ISO-8859-1")
    end
  end

  def test_longer_valid_utf8_string
    s = <<-END
      Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt
      ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco
      laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in
      voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat
      non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
    END
    assert Rejectu.valid? s
  end

  def test_longer_utf8_string
    s = <<-END
      Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt
      ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco
      laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in
      voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat
      non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. \xf2\xa4\xb7\xa4
    END
    refute Rejectu.valid? s
  end

  def test_longer_utf8_string2
    s = <<-END
      Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt
      ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco
      laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in
      voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat
      non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
      \xf2\xa4\xb7\xa4
      Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt
      ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco
      laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in
      voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat
      non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
    END
    refute Rejectu.valid? s
  end

  def test_scrub
    assert_equal "? test string", Rejectu.scrub("\xf2\xa4\xb7\xa4 test string")
  end

  def test_scrub_with_custom_token
    assert_equal ". test string", Rejectu.scrub("\xf2\xa4\xb7\xa4 test string", ".")
  end

  def test_scrub!
    s = "\xf2\xa4\xb7\xa4 test string"
    assert_equal "? test string", Rejectu.scrub!(s)
    assert_equal "? test string", s
  end

  def test_scrub_with_custom_token!
    s = "\xf2\xa4\xb7\xa4 test string"
    assert_equal ". test string", Rejectu.scrub!(s, ".")
    assert_equal ". test string", s
  end
end
