Gem::Specification.new do |s|
  s.name = 'rejectu'
  s.version = '0.0.1'
  s.summary = 'detects if a UTF-8 contains characters that are more than 3 bytes'
  s.description = 'detects if a UTF-8 contains characters that are more than 3 bytes'

  s.homepage = 'https://github.com/csfrancis/rejectu'
  s.authors = 'Scott Francis'
  s.email   = 'scott.francis@shopify.com'
  s.license = 'MIT'

  s.files = `git ls-files`.split("\n")
  s.extensions = ['ext/rejectu/extconf.rb']
  s.add_development_dependency 'rake-compiler', '~> 0.9'
end