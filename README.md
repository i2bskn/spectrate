# Spectrate

Object with continuous range in Ruby.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'spectrate'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install spectrate

## Usage

```ruby
objects = [*1..10].map {|i| Struct.new(:name, :range).new("obj#{i}", rand(10) + 1) }
# => [#<struct  name="obj1", range=9>,
# #<struct  name="obj2", range=9>,
# #<struct  name="obj3", range=2>,
# #<struct  name="obj4", range=8>,
# #<struct  name="obj5", range=6>,
# #<struct  name="obj6", range=1>,
# #<struct  name="obj7", range=4>,
# #<struct  name="obj8", range=9>,
# #<struct  name="obj9", range=5>,
# #<struct  name="obj10", range=6>]

spectrate = Spectrate.new(objects, objects.map(&:range))
spectrate[5..15] # Object of the corresponding range.
# => [#<struct  name="obj1", range=9>, #<struct  name="obj2", range=9>]

spectrate.sample # Sample to match the range of size.
[*1..(objects.map(&:range).inject(&:+) * 10)]
  .map { spectrate.sample }
  .group_by(&:name)
  .each_with_object({}) {|(k,v), obj| obj[k] = v.size }
# => {"obj8"=>93, "obj3"=>24, "obj1"=>90, "obj4"=>77, "obj9"=>50, "obj7"=>38, "obj2"=>92, "obj5"=>64, "obj10"=>53, "obj6"=>9}
```

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake spec` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/i2bskn/spectrate. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [Contributor Covenant](http://contributor-covenant.org) code of conduct.


## License

The gem is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).

