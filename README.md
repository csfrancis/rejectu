## Rejectu

A simple Ruby extension that verifies that a UTF-8 string does not contain any characters from supplementary planes (code points >= `U+10000`).

### Features

- C extension that uses SSE2 for webscale

### Installation

`gem install rejectu`

or if you're using bundler add the following to your Gemfile

`gem "rejectu"`

### Usage

```ruby
require 'rejectu/rejectu'

Rejectu.valid?("happy! \xf2\xa4\xb7\xa4") # false
Rejectu.valid?("really happy!") # true
Rejectu.valid?("this should be good too \xe2\x84\xa2") # true

Rejectu.scrub("happy! \xf2\xa4\xb7\xa4") # => "happy! ?"

# using a custom replacement character
Rejectu.scrub("happy! \xf2\xa4\xb7\xa4", ".") # => "happy! ."
```
