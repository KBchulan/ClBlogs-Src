package service

import "ginLearn/models"

type UserService struct {
}

func NewUserService() *UserService {
	return &UserService{}
}

func (us *UserService) GetUserByName(name string) (*models.User, error) {
	return &models.User{
		ID:    1,
		Name:  "chulan",
		Email: "test@163.com",
	}, nil
}
