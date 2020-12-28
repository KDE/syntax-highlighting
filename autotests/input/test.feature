
Feature: Let’s test Cucumber feature syntax highlighting
  These are just example features to see that syntax highlighting works.

  These features are copied from Cucumber’s documentation, and they are
  licensed under MIT.

  Source repository can be found at:
  https://github.com/cucumber/docs.cucumber.io

  # Comments are not considered to be part of a feature.
  Rule: Users are notified about overdue tasks on first use of the day
    Background:
      Given I have overdue tasks

    Example: First use of the day
      """
      Docstrings can also be used.
      """
      Given I last used the app yesterday
      When I use the app
      Then I am notified about overdue tasks

    Example: Already used today
      Given I last used the app earlier today
      When I use the app
      Then I am not notified about overdue tasks

Feature: Multiple site support
  Only blog owners can post to a blog, except administrators,
  who can post to all blogs.

  Background:
    Given a global administrator named "Greg"
    And a blog named "Greg's anti-tax rants"
    * a customer named "Dr. Bill"
    And a blog named "Expensive Therapy" owned by "Dr. Bill"

  @tagged_scenario
  Scenario: Dr. Bill posts to his own blog
    Given I am logged in as Dr. Bill
    When I try to post to "Expensive Therapy"
    Then I should see "Your article was published."

  Scenario: Dr. Bill tries to post to somebody else's blog, and fails
    Given the following users exist:
      | name   | email              | twitter         |
      | Aslak  | aslak@cucumber.io  | @aslak_hellesoy |
      | Julien | julien@cucumber.io | @jbpros         |
      | Matt   | matt@cucumber.io   | @mattwynne      |
    When I try to post to "Greg's anti-tax rants"
    Then I should see "Hey! That's not your blog!"

Feature: There can also be Scenario Outline

  Scenario Outline: eating
    Given there are <start> cucumbers
    When I eat <eat> cucumbers
    Then I should have <left> cucumbers

    Examples:
      | start | eat | left |
      |    12 |   5 |    7 |
      |    20 |   5 |   15 |

# kate: indent-width 2;