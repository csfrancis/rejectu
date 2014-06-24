## Rejectu

A simple Ruby extension that verifies that a UTF-8 string does not contain any characters from supplementary planes (code points >= `U+10000`).

### Features

- C extension that uses SSE2 for webscale

### Usage

```ruby
require 'rejectu/rejectu'

Rejectu.valid? "happy! \xf2\xa4\xb7\xa4" # false
Rejectu.valid? "really happy!" # true
Rejectu.valid? "this should be good too \xe2\x84\xa2" # true

Rejectu.scrub "happy! \xf2\xa4\xb7\xa4" # => "happy! ?"
```

### Limitations

- The replacement character for `scrub` currently cannot be configured
