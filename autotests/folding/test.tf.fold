# this is a comment
terraform {
  required_providers {
    local = {
      source = "hashicorp/local"
    }
  }
  backend "http" {
  }
}

variable "variable" {
  type    = list(string)
  default = ["a", "b", "c"]
}

locals {
  a = 1
  b = "aaaa"
  c = true
}


provider "aws" {
}

module "module_each" {
  for_each = toset(local.a)
  source   = "module/${var.interpolation}/end"
}

module "module_count" {
  depends_on = [module.module_each]
  count      = 1
  source     = "module/path"
  something  = count.index
}

output "output" {
  value = [local.a, var.a]
}
